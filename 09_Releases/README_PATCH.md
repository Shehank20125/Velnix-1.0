# Patch: FFmpeg.AutoGen version pin fix

## Files changed
- `src/DecodeHost/FFmpegNativeBootstrap.cs`
- `src/DecodeHost/DecodeHost_NOTES.md`

## What was wrong
`DecodeHost.csproj` was already correctly pinned to `FFmpeg.AutoGen 8.1.0`
(confirmed via NuGet: 8.1.0 is the newest published version as of
2026-08-08; no 8.2/9.x exists yet, matching what DecodeHost_NOTES.md's
change log already discovered). That part needed no change.

But `FFmpegNativeBootstrap.cs`'s doc-comment still listed **FFmpeg
6.1-era DLL soname numbers** (avutil-58, avcodec-60, avformat-60,
swscale-7, swresample-4) — stale from before the 6.1.0.4 -> 7.0.0 ->
9.0 -> 8.1.0 version churn recorded in DecodeHost_NOTES.md. If you'd
gone looking for DLLs matching that comment, you'd have grabbed the
wrong FFmpeg release and hit "entry point not found" at first native
call.

## What's fixed
Confirmed the correct FFmpeg 8.1 "Hoare" Windows shared-build DLL names
against current sources (gyan.dev / BtbN 8.1.2 shared packages) and
updated the doc-comment:
- avutil-60.dll
- avcodec-62.dll
- avformat-62.dll
- swresample-6.dll
- swscale-9.dll

These match `tools/DecodeHarness/ffmpeg/PLACEHOLDER_README.md`, which
was already correctly pointed at 8.1.x — that file needed no change.

## What's still NOT verified
This is a documentation/comment fix only. It does not touch the actual
decode logic. Still outstanding, unchanged from before:
- Whether `src/DecodeHost/FFmpegMediaDecoder.cs` compiles clean against
  `FFmpeg.AutoGen` 8.1.0's actual generated API surface — no `dotnet
  build` has been run since the 8.1.0 pin.
- No native FFmpeg DLLs have been placed next to any executable.
- No decode of any file, malformed or real, has been attempted.

## Suggested next step
On the Windows machine: `dotnet build 07_Source_Code\Velnix.sln`, fix
any FFmpeg.AutoGen 8.1.0 API surface errors that come up (channel-layout
calls are the most likely spot), then download an FFmpeg 8.1.x "Hoare"
shared build from gyan.dev and drop the 5 DLLs into
`tools\DecodeHarness\bin\x64\Debug\net8.0-windows10.0.19041.0\ffmpeg\`
before running DecodeHarness.
