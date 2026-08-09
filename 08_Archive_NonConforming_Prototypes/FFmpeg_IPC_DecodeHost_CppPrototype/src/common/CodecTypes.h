#pragma once
// Codec/container coverage locked in during Phase 2 planning:
// containers: MP4, MKV, WebM
// video:      H.264, HEVC, AV1
// audio:      AAC, MP3, Opus, FLAC, AC3
//
// These enums are the process-boundary-safe (plain int) representation used
// in IPC messages between the main app and the Decode Host. Mapping to/from
// FFmpeg's AVCodecID / container names happens inside src/decode, so the
// rest of the codebase never depends on libav* headers directly.

#include <cstdint>

namespace mpe {

enum class Container : uint8_t {
    Unknown = 0,
    Mp4,
    Mkv,
    WebM,
};

enum class VideoCodec : uint8_t {
    Unknown = 0,
    H264,
    HEVC,
    AV1,
};

enum class AudioCodec : uint8_t {
    Unknown = 0,
    AAC,
    MP3,
    Opus,
    FLAC,
    AC3,
};

// Hardware acceleration backends, auto-detected in priority order per the
// Phase 2 plan (best available first, software fallback last).
enum class HwAccel : uint8_t {
    None = 0,      // software fallback
    D3D11VA,
    DXVA2,
    NVDEC,
    QuickSync,
};

} // namespace mpe
