# Phase 2 — not wired in yet

These folders are the seams for a future custom decode/render engine, kept
so the architecture doesn't need to change shape later. **Nothing here is
built by CMake right now** — VelnixPlayer runs entirely on libmpv (see
`apps/VelnixPlayer` and `third_party/libmpv`).

- `src/FFmpeg/` — direct FFmpeg demuxer/decoder wrappers, if/when you
  outgrow libmpv (e.g. need a decode pipeline feature libmpv doesn't expose)
- `src/Renderer/` — custom GPU video-frame rendering (D3D11/Vulkan/Metal),
  replacing libmpv's built-in renderer
- `src/Hardware/` — hardware-acceleration backend selection, if managed
  outside of mpv's own `hwdec=auto`
- `src/Playback/` — custom A/V sync and playback clock, if libmpv's isn't
  sufficient
- `src/Audio/`, `src/Subtitle/` — custom audio output / subtitle rendering
- `apps/DecodeHost/` — a sandboxed, separate-process decoder host (stability
  isolation — a crash in a malformed file's decode doesn't take down the
  whole app). Worth doing eventually; not needed for v1.

Rule of thumb: don't populate any of these until VelnixPlayer is a working,
shipped product on libmpv and you've hit something libmpv genuinely can't do.
