// Standalone CLI to exercise Decoder end-to-end without the full app/Qt
// stack. Not part of the shipped product - dev/test tool only.
//
// Usage: decode_probe <media file> [num_frames_to_walk]
// Dumps the first decoded frame to first_frame.ppm for visual verification.

#include <fstream>
#include <iostream>
#include "../../src/decode/Decoder.h"
#include "../../src/decode/HwAccelDetector.h"

namespace {
const char* containerName(mpe::Container c) {
    switch (c) {
        case mpe::Container::Mp4: return "MP4";
        case mpe::Container::Mkv: return "MKV";
        case mpe::Container::WebM: return "WebM";
        default: return "Unknown";
    }
}
const char* videoCodecName(mpe::VideoCodec c) {
    switch (c) {
        case mpe::VideoCodec::H264: return "H.264";
        case mpe::VideoCodec::HEVC: return "HEVC";
        case mpe::VideoCodec::AV1: return "AV1";
        default: return "Unknown";
    }
}
} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: decode_probe <media file> [num_frames]\n";
        return 1;
    }
    std::string path = argv[1];
    int numFrames = argc > 2 ? std::stoi(argv[2]) : 5;

    mpe::HwAccelDetector hwDetector;
    mpe::Decoder decoder(hwDetector);

    if (!decoder.open(path)) {
        std::cerr << "failed to open " << path << "\n";
        return 1;
    }

    std::cout << "container: " << containerName(decoder.container()) << "\n";
    std::cout << "video codec: " << videoCodecName(decoder.videoCodec()) << "\n";
    std::cout << "hw backend: " << (decoder.activeBackend() == mpe::HwAccel::None
                                     ? "none (software)" : "hw") << "\n";

    int decoded = 0;
    mpe::FrameDescriptor frame;
    while (decoded < numFrames && decoder.decodeNextFrame(frame)) {
        std::cout << "frame " << decoded
                  << " pts_us=" << frame.presentationTimestampUs
                  << " " << frame.width << "x" << frame.height
                  << " bytes=" << frame.sharedMemorySize << "\n";

        if (decoded == 0) {
            size_t size = 0;
            const uint8_t* data = decoder.currentFrameData(size);
            if (data) {
                std::ofstream out("first_frame.ppm", std::ios::binary);
                out << "P6\n" << frame.width << " " << frame.height << "\n255\n";
                out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
                std::cout << "wrote first_frame.ppm\n";
            }
        }
        ++decoded;
    }

    if (decoded == 0) {
        std::cerr << "no frames decoded\n";
        return 1;
    }

    std::cout << "decoded " << decoded << " frame(s) successfully\n";
    return 0;
}
