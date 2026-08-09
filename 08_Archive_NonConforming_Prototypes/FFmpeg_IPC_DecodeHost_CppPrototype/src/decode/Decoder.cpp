#include "Decoder.h"

#include <cstring>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
}

namespace mpe {

namespace {

// Maps FFmpeg's demuxer short name(s) to our Container enum. A single
// container can report multiple names (e.g. webm shares the matroska
// demuxer), so this checks substrings rather than exact match.
Container mapContainer(const AVFormatContext* fmt) {
    if (!fmt || !fmt->iformat || !fmt->iformat->name) return Container::Unknown;
    std::string name = fmt->iformat->name;
    if (name.find("webm") != std::string::npos) return Container::WebM;
    if (name.find("matroska") != std::string::npos) return Container::Mkv;
    if (name.find("mp4") != std::string::npos || name.find("mov") != std::string::npos)
        return Container::Mp4;
    return Container::Unknown;
}

VideoCodec mapVideoCodec(AVCodecID id) {
    switch (id) {
        case AV_CODEC_ID_H264: return VideoCodec::H264;
        case AV_CODEC_ID_HEVC: return VideoCodec::HEVC;
        case AV_CODEC_ID_AV1:  return VideoCodec::AV1;
        default: return VideoCodec::Unknown;
    }
}

AudioCodec mapAudioCodec(AVCodecID id) {
    switch (id) {
        case AV_CODEC_ID_AAC:  return AudioCodec::AAC;
        case AV_CODEC_ID_MP3:  return AudioCodec::MP3;
        case AV_CODEC_ID_OPUS: return AudioCodec::Opus;
        case AV_CODEC_ID_FLAC: return AudioCodec::FLAC;
        case AV_CODEC_ID_AC3:  return AudioCodec::AC3;
        default: return AudioCodec::Unknown;
    }
}

} // namespace

struct Decoder::Impl {
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* videoCodecCtx = nullptr;
    AVCodecContext* audioCodecCtx = nullptr;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;

    SwsContext* swsCtx = nullptr;
    std::vector<uint8_t> frameBuffer; // packed RGB24 output of the last decoded frame

    AVPacket* packet = nullptr;
    AVFrame* decodedFrame = nullptr;
    AVFrame* rgbFrame = nullptr;

    ~Impl() {
        if (swsCtx) sws_freeContext(swsCtx);
        if (packet) av_packet_free(&packet);
        if (decodedFrame) av_frame_free(&decodedFrame);
        if (rgbFrame) av_frame_free(&rgbFrame);
        if (videoCodecCtx) avcodec_free_context(&videoCodecCtx);
        if (audioCodecCtx) avcodec_free_context(&audioCodecCtx);
        if (formatCtx) avformat_close_input(&formatCtx);
    }
};

Decoder::Decoder(HwAccelDetector& hwDetector)
    : hwDetector_(hwDetector), impl_(std::make_unique<Impl>()) {}

Decoder::~Decoder() = default;

bool Decoder::open(const std::string& uri) {
    close();

    AVFormatContext* fmtCtx = nullptr;
    if (avformat_open_input(&fmtCtx, uri.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Decoder::open: avformat_open_input failed for " << uri << "\n";
        return false;
    }
    if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
        std::cerr << "Decoder::open: avformat_find_stream_info failed\n";
        avformat_close_input(&fmtCtx);
        return false;
    }
    impl_->formatCtx = fmtCtx;
    container_ = mapContainer(fmtCtx);

    // --- Video stream ---------------------------------------------------
    const AVCodec* videoDecoder = nullptr;
    int videoIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
    if (videoIdx < 0) {
        std::cerr << "Decoder::open: no video stream found\n";
        close();
        return false;
    }
    AVCodecParameters* videoParams = fmtCtx->streams[videoIdx]->codecpar;
    videoCodec_ = mapVideoCodec(videoParams->codec_id);
    if (videoCodec_ == VideoCodec::Unknown) {
        std::cerr << "Decoder::open: video codec outside locked-in coverage "
                     "(H.264/HEVC/AV1) - codec_id=" << videoParams->codec_id << "\n";
        close();
        return false;
    }

    AVCodecContext* videoCtx = avcodec_alloc_context3(videoDecoder);
    if (!videoCtx || avcodec_parameters_to_context(videoCtx, videoParams) < 0) {
        std::cerr << "Decoder::open: failed to set up video codec context\n";
        close();
        return false;
    }

    // NOTE: software decode only for now. Hardware acceleration
    // (D3D11VA/DXVA2/NVDEC/QuickSync) selection via hwDetector_ is not
    // wired into the AVCodecContext yet - see HwAccelDetector.cpp TODOs.
    // We still consult it here so the call site this method will eventually
    // need is already in place.
    HwAccel chosen = hwDetector_.bestAvailable();
    activeBackend_ = HwAccel::None; // stays None until hw path is implemented
    (void)chosen;

    if (avcodec_open2(videoCtx, videoDecoder, nullptr) < 0) {
        std::cerr << "Decoder::open: avcodec_open2 (video) failed\n";
        avcodec_free_context(&videoCtx);
        close();
        return false;
    }
    impl_->videoCodecCtx = videoCtx;
    impl_->videoStreamIndex = videoIdx;

    // --- Audio stream (opened for future decode; not decoded yet) -------
    const AVCodec* audioDecoder = nullptr;
    int audioIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, videoIdx, &audioDecoder, 0);
    if (audioIdx >= 0) {
        AVCodecParameters* audioParams = fmtCtx->streams[audioIdx]->codecpar;
        audioCodec_ = mapAudioCodec(audioParams->codec_id);
        // Not a hard failure if unsupported/unknown - video-only playback
        // of the happy path is still useful for validating the pipeline.
        AVCodecContext* audioCtx = avcodec_alloc_context3(audioDecoder);
        if (audioCtx && avcodec_parameters_to_context(audioCtx, audioParams) >= 0 &&
            avcodec_open2(audioCtx, audioDecoder, nullptr) >= 0) {
            impl_->audioCodecCtx = audioCtx;
            impl_->audioStreamIndex = audioIdx;
        } else {
            if (audioCtx) avcodec_free_context(&audioCtx);
            std::cerr << "Decoder::open: audio stream present but could not "
                         "open codec - continuing video-only\n";
        }
    }

    impl_->packet = av_packet_alloc();
    impl_->decodedFrame = av_frame_alloc();
    impl_->rgbFrame = av_frame_alloc();
    if (!impl_->packet || !impl_->decodedFrame || !impl_->rgbFrame) {
        std::cerr << "Decoder::open: failed to allocate packet/frame buffers\n";
        close();
        return false;
    }

    return true;
}

void Decoder::close() {
    impl_ = std::make_unique<Impl>();
    container_ = Container::Unknown;
    videoCodec_ = VideoCodec::Unknown;
    audioCodec_ = AudioCodec::Unknown;
    activeBackend_ = HwAccel::None;
}

bool Decoder::decodeNextFrame(FrameDescriptor& outFrame) {
    if (!impl_->formatCtx || !impl_->videoCodecCtx) return false;

    while (true) {
        int readResult = av_read_frame(impl_->formatCtx, impl_->packet);
        if (readResult < 0) {
            return false; // EOF or read error
        }

        if (impl_->packet->stream_index != impl_->videoStreamIndex) {
            // Audio (or other) packet - not decoded yet, discard.
            av_packet_unref(impl_->packet);
            continue;
        }

        int sendResult = avcodec_send_packet(impl_->videoCodecCtx, impl_->packet);
        av_packet_unref(impl_->packet);
        if (sendResult < 0) {
            std::cerr << "Decoder::decodeNextFrame: avcodec_send_packet failed\n";
            continue;
        }

        int recvResult = avcodec_receive_frame(impl_->videoCodecCtx, impl_->decodedFrame);
        if (recvResult == AVERROR(EAGAIN)) {
            continue; // needs another packet
        }
        if (recvResult < 0) {
            std::cerr << "Decoder::decodeNextFrame: avcodec_receive_frame failed\n";
            continue;
        }

        // Got a decoded frame. Convert to packed RGB24 for now (see
        // Decoder.h note on why RGB24 rather than NV12 at this stage).
        AVFrame* src = impl_->decodedFrame;
        int width = src->width;
        int height = src->height;

        if (!impl_->swsCtx) {
            impl_->swsCtx = sws_getContext(
                width, height, static_cast<AVPixelFormat>(src->format),
                width, height, AV_PIX_FMT_RGB24,
                SWS_BILINEAR, nullptr, nullptr, nullptr);
            if (!impl_->swsCtx) {
                std::cerr << "Decoder::decodeNextFrame: sws_getContext failed\n";
                return false;
            }
        }

        impl_->frameBuffer.resize(static_cast<size_t>(width) * height * 3);
        uint8_t* dstData[4] = { impl_->frameBuffer.data(), nullptr, nullptr, nullptr };
        int dstLinesize[4] = { width * 3, 0, 0, 0 };

        sws_scale(impl_->swsCtx, src->data, src->linesize, 0, height, dstData, dstLinesize);

        AVRational streamTimeBase = impl_->formatCtx->streams[impl_->videoStreamIndex]->time_base;
        int64_t ptsUs = 0;
        if (src->pts != AV_NOPTS_VALUE) {
            ptsUs = av_rescale_q(src->pts, streamTimeBase, AVRational{1, 1000000});
        }

        outFrame.presentationTimestampUs = static_cast<uint64_t>(ptsUs);
        outFrame.width = static_cast<uint32_t>(width);
        outFrame.height = static_cast<uint32_t>(height);
        outFrame.storage = FrameStorage::SharedMemory;
        outFrame.producedBy = activeBackend_;
        outFrame.sharedMemoryHandle = 0; // TODO: real shared memory (see decode/README.md)
        outFrame.sharedMemorySize = static_cast<uint32_t>(impl_->frameBuffer.size());
        outFrame.sharedTextureHandle = 0;

        av_frame_unref(impl_->decodedFrame);
        return true;
    }
}

const uint8_t* Decoder::currentFrameData(size_t& outSize) const {
    outSize = impl_->frameBuffer.size();
    return impl_->frameBuffer.empty() ? nullptr : impl_->frameBuffer.data();
}

} // namespace mpe
