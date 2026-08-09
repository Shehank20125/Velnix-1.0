#pragma once
// Frame delivery descriptor passed from the Decode Host process to the main
// app process. Per the Phase 2 plan, frames are delivered via shared memory
// (software-decoded frames) or a shared D3D11 texture handle (hw-decoded
// frames) to avoid a per-frame copy across the process boundary.
//
// STUB NOTE: the actual handle-passing mechanism (DuplicateHandle + IPC
// message carrying the HANDLE value, vs. a named shared-texture resource)
// is not yet decided. This struct captures the shape of the data; the IPC
// transport that carries it is stubbed in src/ipc.

#include <cstdint>
#include "CodecTypes.h"

namespace mpe {

enum class FrameStorage : uint8_t {
    SharedMemory = 0,   // software-decoded path
    SharedTexture = 1,  // hw-accelerated path (D3D11 texture handle)
};

struct FrameDescriptor {
    uint64_t presentationTimestampUs = 0; // PTS in microseconds
    uint32_t width = 0;
    uint32_t height = 0;
    FrameStorage storage = FrameStorage::SharedMemory;
    HwAccel producedBy = HwAccel::None;

    // Valid when storage == SharedMemory: name/handle of the shared memory
    // segment holding the raw frame data (format TBD: NV12 likely for
    // hw-decoded parity, see decode/README.md).
    uint64_t sharedMemoryHandle = 0;
    uint32_t sharedMemorySize = 0;

    // Valid when storage == SharedTexture: OS handle to the shared D3D11
    // texture. Actual duplication/opening across processes is stubbed —
    // see src/render/FrameSink.h.
    uint64_t sharedTextureHandle = 0;
};

} // namespace mpe
