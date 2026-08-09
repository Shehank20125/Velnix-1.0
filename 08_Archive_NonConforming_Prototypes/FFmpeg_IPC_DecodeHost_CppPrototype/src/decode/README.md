# Decode subsystem — status

## Implemented (real, tested against actual media files)

- `Decoder::open()` — opens the container via `avformat_open_input`, probes
  streams, selects the best video stream (and audio stream if present),
  maps codec IDs to `mpe::VideoCodec` / `mpe::AudioCodec`, and rejects
  video codecs outside the locked-in coverage (H.264/HEVC/AV1). Audio
  outside the locked-in set (AAC/MP3/Opus/FLAC/AC3) is tolerated for now —
  falls back to video-only rather than a hard failure, since audio decode
  isn't implemented yet anyway.
- `Decoder::decodeNextFrame()` — real `av_read_frame` / `avcodec_send_packet`
  / `avcodec_receive_frame` loop for the video stream. Converts each frame
  to packed RGB24 via `sws_scale` and returns a populated `FrameDescriptor`.
- PTS conversion from the stream's time_base to microseconds.

## Not implemented yet

- **Hardware acceleration.** `Decoder::open()` calls
  `hwDetector_.bestAvailable()` but doesn't act on the result —
  `activeBackend_` is always `HwAccel::None` and decode is always software.
  `HwAccelDetector` itself doesn't probe real hw device contexts yet either
  (see `HwAccelDetector.cpp` TODOs). Wiring a `AVHWDeviceContext` into the
  `AVCodecContext` for D3D11VA/DXVA2/NVDEC/QuickSync is the next real chunk
  of work here.
- **Audio decode.** Audio packets are currently read and discarded in
  `decodeNextFrame()`. No audio `avcodec_send_packet`/`receive_frame` loop,
  no resampling, no audio `FrameDescriptor` output.
- **Real shared-memory / shared-texture delivery.** Decoded RGB24 data
  currently lives in an internal buffer owned by `Decoder`
  (`currentFrameData()`), not in actual OS shared memory —
  `FrameDescriptor::sharedMemoryHandle` is always `0`. This is intentionally
  deferred until the IPC transport is chosen (`src/ipc/README.md`), since
  the handle-passing mechanism depends on that choice. The buffer approach
  lets the decode pipeline be tested standalone in the meantime (see
  `tools/decode_probe`).
- **Output pixel format.** RGB24 was chosen for now purely because it's
  trivial to dump to a `.ppm` and eyeball-verify. `FrameTypes.h` already
  flags that the real wire format is likely NV12 for parity with the
  hw-decode path — switching `decodeNextFrame()`'s `sws_getContext` target
  format is a one-line change when that's decided, but downstream
  consumers (once they exist) should not assume RGB24 is final.
- **Seeking.** No `av_seek_frame` support yet.

## Verified against

`tools/decode_probe` (small standalone CLI, not part of the shipped app)
decodes a real file end-to-end and dumps the first frame to a `.ppm` to
confirm the pipeline produces correct pixel data, not just that it compiles.
