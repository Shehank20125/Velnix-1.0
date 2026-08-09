# DecodeHost — setup & verification status

## What this is

The real Phase 2 Core Engine: `FFmpegMediaDecoder` implements `Playback.IMediaDecoder`
using [FFmpeg.AutoGen](https://github.com/Ruslan-B/FFmpeg.AutoGen) — bindings generated
directly from FFmpeg's C headers, rather than hand-written P/Invoke structs. That choice
matters here specifically: `AVFrame`/`AVCodecContext` are large, version-sensitive
structs, and a hand-rolled field offset that's wrong doesn't fail loudly — it corrupts
memory silently. Generated bindings tied to a specific FFmpeg version avoid that failure
mode.

## Before this compiles or runs

1. **NuGet restore** pulls `FFmpeg.AutoGen` (managed P/Invoke signatures only — no native
   binaries).
2. **You must separately place an LGPL-configured FFmpeg shared build** next to the app
   executable, in an `ffmpeg\` subfolder (see `FFmpegNativeBootstrap.cs`):
   `avutil-*.dll`, `avcodec-*.dll`, `avformat-*.dll`, `swscale-*.dll`, `swresample-*.dll`.
   Get these from an LGPL shared build (e.g. gyan.dev's "shared" builds, or build FFmpeg
   yourself with `--disable-gpl --enable-shared`) — **not** a GPL or static build, per
   Constitution v1.1's licensing policy. Confirm the DLL version matches what
   `FFmpeg.AutoGen` 6.1.0.4 expects; mismatches typically fail at first call with a clear
   entry-point-not-found error rather than something subtle.
3. Confirm dynamic (not static) linking end-to-end before shipping anything — that's a
   hard Constitution requirement, not a style preference.

## Verification status — read this before trusting any of it

**Now pinned to `FFmpeg.AutoGen` 8.1.0 (confirmed available on NuGet — 9.0 does not
exist yet, see change log below), paired with an FFmpeg 8.1 "Hoare" native shared
build.** NOT yet rebuilt or re-run against this. The channel-layout code here
(`swr_alloc_set_opts2`, `AVChannelLayout`) already targets the FFmpeg 5.1+ API surface,
which predates and should be unaffected by moving between 7.0.0/8.1.0/9.0, so no source
changes were made — but that's an expectation, not a verified fact, until it's actually
built. Still unverified:

- Whether it still compiles clean against `FFmpeg.AutoGen` 8.1.0's actual generated API —
  do a `dotnet build` pass first and treat any errors here as the first thing to fix.

- Real playback of any file, malformed or otherwise — no native FFmpeg DLLs have been
  placed next to the executable yet, and no run has been attempted.
- Whether the hostile-input paths actually throw `MediaParsingException` cleanly (this is
  a hard requirement per `Threat_Model.md` item 1 — a fuzz/malformed-file pass belongs
  here before anything else ships).
- A/V sync behavior in `PlaybackController.PumpLoopAsync` beyond it looking correct on
  paper — there's no prefetch/buffering depth yet, so stutter under real decode load is
  expected until that's added.

## Suggested first real session against this

1. Get an LGPL FFmpeg build in place, restore NuGet, and just get `dotnet build` clean —
   expect FFmpeg.AutoGen API surface mismatches on the first attempt.
2. Write a throwaway console harness (bypass WinUI/DI entirely) that opens a known-good
   MP4 and dumps `MediaInfo` plus the first few frame timestamps to the console. Cheapest
   way to find struct/enum mismatches.
3. Only once that works, wire it through `CompositionRoot` into the real app and swap
   `NullVideoRenderer`/`NullAudioOutput` for real WinUI/WASAPI implementations (not
   attempted here — see their doc-comments).

## Change log

- `FFmpeg.AutoGen` bumped `6.1.0.4` → `7.0.0` (2026-08-04): the `6.1.0.4` version string
  doesn't exist on NuGet, so it was silently floating to `7.0.0` anyway (NU1603 warning).
  Pinned explicitly since the build already confirmed 7.0.0's API surface matches the
  code as written. No source changes were needed.
- `FFmpeg.AutoGen` bumped `7.0.0` → `9.0` (2026-08-08): matches the native FFmpeg 9.0
  "Lei" shared build being used. No source changes made — the code already targets the
  post-5.1 channel-layout API which 9.0 doesn't touch — but **this bump has not been
  rebuilt or run yet**, so treat the "compiles clean" claim above as stale until a fresh
  `dotnet build` confirms it against 9.0. This is the first thing to verify next.
- `FFmpeg.AutoGen` reverted `9.0` → `8.1.0` (2026-08-08, same day): **`9.0` does not
  exist on NuGet** — confirmed via a real `dotnet restore` failure (`Unable to find
  package FFmpeg.AutoGen with version (>= 9.0.0)`, nuget.org reports 61 published
  versions, nearest is `8.1.0`). `8.1.0` is the newest version actually published.
  **This means the FFmpeg 9.0 "Lei" native DLLs already downloaded (avutil-61,
  avcodec-63, avformat-63, etc.) do NOT match** — need to swap for an FFmpeg **8.1
  "Hoare"** shared build from gyan.dev instead. Soname numbers will differ from the 9.0
  set; get whatever the 8.1 gyan.dev build actually names them rather than assuming.
- **Pin confirmed correct (2026-08-08):** `FFmpeg.AutoGen` 8.1.0 is still the newest
  version published on NuGet (no 8.2/9.x exists), so no further version bump is needed —
  `DecodeHost.csproj`'s `<PackageReference Include="FFmpeg.AutoGen" Version="8.1.0" />`
  is correct as-is. The matching FFmpeg 8.1 "Hoare" Windows shared-build DLL names are:
  `avutil-60.dll`, `avcodec-62.dll`, `avformat-62.dll`, `swresample-6.dll`,
  `swscale-9.dll` (confirmed against current gyan.dev / BtbN 8.1.x shared packages — the
  minor 8.0→8.1 release didn't bump any library sonames). `FFmpegNativeBootstrap.cs`'s
  doc-comment was still listing FFmpeg 6.1-era names from before this file's version
  churn; that's now fixed to match. **Still nothing has actually been built or run** —
  this only resolves which package/DLL versions to use, not whether the code compiles
  against them.
