// Decode Host: an isolated process that owns all FFmpeg decoding, per
// Threat Model item 1 in the Phase 2 plan. Communicates with the main app
// process over IpcChannel (transport TBD, see src/ipc/README.md) and
// delivers decoded frames via shared memory or shared D3D11 texture handles
// (see src/common/FrameTypes.h).
//
// This process should be launched with restricted privileges/sandboxing
// appropriate to its job (parses untrusted, potentially malicious media
// files) — sandboxing policy itself is not scoped for Phase 2 per the
// planning doc and should be tracked separately if not already.

#include <iostream>
#include "../decode/Decoder.h"
#include "../decode/HwAccelDetector.h"
#include "../ipc/IpcChannel.h"

int main(int argc, char** argv) {
    mpe::HwAccelDetector hwDetector;
    mpe::Decoder decoder(hwDetector);
    mpe::StubIpcChannel ipc; // TODO: replace with the chosen real transport

    if (!ipc.connect()) {
        std::cerr << "decode_host: failed to connect IPC channel to main app\n";
        return 1;
    }

    // TODO: message loop — receive "open this URI" / "seek" / "decode next
    // frame" requests over ipc, drive `decoder` accordingly, and push
    // FrameDescriptor results back over ipc as they're produced.
    (void)argc;
    (void)argv;

    return 0;
}
