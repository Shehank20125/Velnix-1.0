#include "HwAccelDetector.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace mpe {

namespace {
// Maps our HwAccel enum to the FFmpeg AVHWDeviceType used to probe it.
// TODO: fill in once probing is implemented — e.g.
//   D3D11VA  -> AV_HWDEVICE_TYPE_D3D11VA
//   DXVA2    -> AV_HWDEVICE_TYPE_DXVA2
//   NVDEC    -> AV_HWDEVICE_TYPE_CUDA
//   QuickSync-> AV_HWDEVICE_TYPE_QSV
} // namespace

std::vector<HwAccel> HwAccelDetector::detectAvailableBackends() {
    if (probed_) return cachedBackends_;

    cachedBackends_.clear();
    for (HwAccel candidate : kProbeOrder) {
        // TODO: actually attempt av_hwdevice_ctx_create() for the mapped
        // AVHWDeviceType and record success/failure. Stubbed to "none found"
        // so the software fallback path is exercised end-to-end for now.
        (void)candidate;
    }

    probed_ = true;
    return cachedBackends_;
}

HwAccel HwAccelDetector::bestAvailable() {
    auto backends = detectAvailableBackends();
    return backends.empty() ? HwAccel::None : backends.front();
}

bool HwAccelDetector::supportsStream(HwAccel backend, VideoCodec codec,
                                     uint32_t width, uint32_t height) {
    // TODO: real per-title check (profile/level limits, resolution caps for
    // the given backend, chroma format support, etc.).
    (void)codec;
    (void)width;
    (void)height;
    return backend != HwAccel::None;
}

} // namespace mpe
