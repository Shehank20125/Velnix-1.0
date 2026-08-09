#pragma once
// Auto-detects the best available hardware acceleration backend at startup,
// per the Phase 2 decision: probe in priority order, software fallback if
// none are available or if per-title probing fails (some titles may not be
// supported by the detected hw backend even if the backend itself works).

#include <vector>
#include "../common/CodecTypes.h"

namespace mpe {

class HwAccelDetector {
public:
    // Probes the system once (expensive-ish: creates/destroys test decoder
    // contexts) and caches the result. Call at Decode Host startup.
    std::vector<HwAccel> detectAvailableBackends();

    // Returns the highest-priority backend from detectAvailableBackends(),
    // or HwAccel::None if nothing usable was found.
    HwAccel bestAvailable();

    // Per-title check: even if a backend is generally available, a specific
    // stream (unusual profile/level, odd chroma subsampling, etc.) may need
    // to fall back to software. Called once per opened stream.
    bool supportsStream(HwAccel backend, VideoCodec codec,
                         uint32_t width, uint32_t height);

private:
    // Priority order per Phase 2 plan: D3D11VA/DXVA2/NVDEC/QuickSync via
    // FFmpeg, software last.
    static constexpr HwAccel kProbeOrder[] = {
        HwAccel::D3D11VA, HwAccel::DXVA2, HwAccel::NVDEC, HwAccel::QuickSync,
    };

    std::vector<HwAccel> cachedBackends_;
    bool probed_ = false;
};

} // namespace mpe
