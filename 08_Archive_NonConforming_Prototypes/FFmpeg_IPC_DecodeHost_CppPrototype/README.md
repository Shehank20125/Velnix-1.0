# Phase 2 — Core Engine (implementation scaffold)

This is a build-able skeleton for the Phase 2 plan: two-process
architecture (main app + isolated Decode Host), FFmpeg-based decoding with
auto-detected hardware acceleration, and shared-memory/shared-texture frame
delivery.

## Layout

```
src/
  common/       Shared POD types (codec enums, frame descriptors) safe to
                use on both sides of the process boundary.
  ipc/          IpcChannel interface between app <-> decode_host.
                TRANSPORT NOT CHOSEN YET — see src/ipc/README.md.
                Currently wired up with a no-op StubIpcChannel.
  decode/       FFmpeg wrapper (Decoder) + hw-accel auto-detection
                (HwAccelDetector). Only ever linked into decode_host.
  render/       FrameSink interface the UI uses to display decoded frames.
                Currently wired up with a no-op NullFrameSink.
  app/          Main Qt application (mpe_app). Never links FFmpeg directly.
  decode_host/  Isolated process (mpe_decode_host) that owns all decoding.
```

## What's real vs. stubbed

**Real / structural** (encodes locked-in Phase 2 decisions):
- Two-executable split (`mpe_app`, `mpe_decode_host`) with `mpe_decode`
  (FFmpeg) only linkable from the host, enforced by CMake dependency graph.
- Codec/container/hw-accel enums matching the locked-in coverage
  (MP4/MKV/WebM; H.264/HEVC/AV1; AAC/MP3/Opus/FLAC/AC3; D3D11VA/DXVA2/NVDEC/
  QuickSync + software fallback).
- `FrameDescriptor` shape distinguishing shared-memory vs. shared-texture
  delivery.

**Real / working** (implemented against actual FFmpeg and verified against
real H.264 and HEVC MP4 files — not just compiled, actually run):
- `Decoder::open()` — opens a file, probes container/codec, maps to our
  enums, rejects video codecs outside the locked-in coverage.
- `Decoder::decodeNextFrame()` — real demux/decode loop, produces correctly
  timestamped `FrameDescriptor`s with real pixel data (currently RGB24 —
  see `src/decode/README.md` for why and what changes later).
- `tools/decode_probe` — a small standalone CLI (not part of the shipped
  app) that exercises the above end-to-end and dumps a frame to `.ppm` for
  visual verification. Build it and point it at any MP4 to try it yourself:
  `cmake --build build --target decode_probe && ./build/tools/decode_probe/decode_probe yourfile.mp4`

**Still stubbed / open** (needs a decision or real implementation):
- **IPC transport** — named pipes/sockets vs. gRPC, see `src/ipc/README.md`.
  This was flagged as not decided; `StubIpcChannel` is a no-op placeholder.
- **Hardware acceleration** — `HwAccelDetector` doesn't call
  `av_hwdevice_ctx_create` yet, and `Decoder` doesn't act on it even though
  it asks; decode is always software right now. See `src/decode/README.md`.
- **Audio decode** — audio packets are read and discarded; no
  send/receive-frame loop, no resampling, no audio output.
- **Real shared-memory/shared-texture delivery** — decoded frame data lives
  in an internal buffer for now (`Decoder::currentFrameData()`), not actual
  OS shared memory. Deferred on purpose until the IPC transport is chosen.
- `FrameSink` — `NullFrameSink` does nothing; real Direct3D/Qt paint
  integration and cross-process shared-texture opening not written.
- Decode Host process spawning/lifecycle (crash restart policy, sandboxing)
  from `mpe_app`.
- Seeking.

## Building

Requires Qt6, FFmpeg dev packages (avcodec/avformat/avutil/swscale) 
discoverable via pkg-config, and a C++20 compiler.

```
cmake -B build -S .
cmake --build build
```

## Suggested next steps

1. ~~Implement `Decoder::open`/`decodeNextFrame` for the MP4/H.264 happy
   path.~~ Done — see `src/decode/README.md` for exactly what's covered.
2. Decide the IPC transport (see `src/ipc/README.md`) — this unblocks
   wiring frame delivery end-to-end.
3. Implement `HwAccelDetector::detectAvailableBackends` for real, then wire
   the chosen backend into `Decoder::open`'s `AVCodecContext` setup.
4. Land a real `FrameSink` for the software-decode path (shared memory ->
   texture upload) before tackling the shared-D3D11-texture path, since the
   latter depends on the IPC handle-passing decision.
5. Audio decode loop (currently packets are discarded).
