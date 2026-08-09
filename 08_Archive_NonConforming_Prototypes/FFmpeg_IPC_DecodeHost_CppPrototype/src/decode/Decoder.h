#pragma once
// Wraps FFmpeg demuxing + decoding for a single opened media title. Runs
// exclusively inside the Decode Host process (see src/decode_host) — the
// main app process never links libav* directly, by design (Threat Model
// item 1: decode isolation).

#include <memory>
#include <string>
#include "../common/CodecTypes.h"
#include "../common/FrameTypes.h"
#include "HwAccelDetector.h"

namespace mpe {

class Decoder {
public:
    explicit Decoder(HwAccelDetector& hwDetector);
    ~Decoder();

    // Opens the given file/URI, probes container + codecs, and picks a
    // hw-accel backend (falling back to software per-title if needed).
    // Returns false on failure (unsupported codec, corrupt file, etc.).
    bool open(const std::string& uri);

    void close();

    Container container() const { return container_; }
    VideoCodec videoCodec() const { return videoCodec_; }
    AudioCodec audioCodec() const { return audioCodec_; }
    HwAccel activeBackend() const { return activeBackend_; }

    // Decodes and returns the next VIDEO frame descriptor. Internally skips
    // over audio packets for now — audio decode/output is not implemented
    // yet (see decode/README.md). Returns false at EOF or on error.
    //
    // NOTE ON DELIVERY: real cross-process shared-memory / shared-D3D11-
    // texture handoff depends on the still-open IPC transport decision
    // (src/ipc/README.md). Until that's chosen, the decoded frame's pixel
    // data is kept in an internal buffer owned by this Decoder and exposed
    // via currentFrameData() below, so the pipeline is independently
    // testable in-process. FrameDescriptor::sharedMemoryHandle is a
    // placeholder (0) until real shared memory is wired in.
    bool decodeNextFrame(FrameDescriptor& outFrame);

    // Raw pixel data for the frame most recently produced by
    // decodeNextFrame(), valid until the next call. Currently packed as
    // RGB24 (chosen for easy verification/dumping to disk); the final wire
    // format (likely NV12, to match the hw-decode path) is still open per
    // FrameTypes.h.
    const uint8_t* currentFrameData(size_t& outSize) const;

private:
    HwAccelDetector& hwDetector_;
    Container container_ = Container::Unknown;
    VideoCodec videoCodec_ = VideoCodec::Unknown;
    AudioCodec audioCodec_ = AudioCodec::Unknown;
    HwAccel activeBackend_ = HwAccel::None;

    // TODO: AVFormatContext*, AVCodecContext* (video/audio), SwsContext*
    // for software fallback color conversion, etc. Opaque'd out for now so
    // this header doesn't force every includer to see libav* types.
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mpe
