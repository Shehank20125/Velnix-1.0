#pragma once
// FrameSink is how the main app's UI consumes frames delivered from the
// Decode Host process. Two paths per the Phase 2 plan:
//   - shared memory: software-decoded frames, sink copies/uploads to a
//     texture for display.
//   - shared D3D11 texture: hw-decoded frames, sink opens the shared
//     handle directly (no CPU copy) and presents it.
//
// STUB NOTE: opening a shared D3D11 texture handle from another process
// (ID3D11Device::OpenSharedResource / OpenSharedResource1) is not yet
// implemented — depends on the IPC transport decision (see src/ipc/README.md)
// for how the handle itself is transmitted/duplicated across processes.

#include "../common/FrameTypes.h"

namespace mpe {

class FrameSink {
public:
    virtual ~FrameSink() = default;

    // Called on receipt of a new frame descriptor from the IPC layer.
    // Implementation should present it (or queue it for the next paint).
    virtual void submitFrame(const FrameDescriptor& frame) = 0;

    // Release any resources tied to a previously submitted frame's shared
    // memory/texture handle once the UI is done displaying it, so the
    // Decode Host can reuse the buffer.
    virtual void releaseFrame(const FrameDescriptor& frame) = 0;
};

// No-op placeholder so the app builds/runs before real Direct3D/Qt paint
// integration is written.
class NullFrameSink : public FrameSink {
public:
    void submitFrame(const FrameDescriptor&) override {}
    void releaseFrame(const FrameDescriptor&) override {}
};

} // namespace mpe
