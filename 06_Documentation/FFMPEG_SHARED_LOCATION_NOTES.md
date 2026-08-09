# FFmpeg Shared Location (2026-08-08)

## Problem
`DecodeHost.csproj` used `CopyToOutputDirectory`, which propagates
transitively through every `ProjectReference` chain. Result: the 5 LGPL
FFmpeg DLLs (~134 MB) were physically duplicated into 3 separate `bin`
folders — `Velnix.App`, `tools/DecodeHarness`, and `DecodeHost` itself —
on every local build.

## Fix
- One canonical copy only: `07_Source_Code/src/DecodeHost/ffmpeg/`
  (already gitignored, never committed, per Constitution's LGPL
  dynamic-linking requirement).
- `DecodeHost.csproj` no longer copies these DLLs anywhere.
- `FFmpegNativeBootstrap.Initialize()` now checks, in order:
  1. `VELNIX_FFMPEG_DIR` environment variable
  2. `ffmpeg\` folder next to the running exe (packaged/shipped case)
  3. `AppContext.BaseDirectory` (last resort)

## One-time dev machine setup
Set the environment variable once (persists across reboots):

```powershell
setx VELNIX_FFMPEG_DIR "C:\path\to\Velnix\07_Source_Code\src\DecodeHost\ffmpeg"
```

Restart Visual Studio / terminal after `setx` so the new value is picked
up. `Velnix.App`, `DecodeHarness`, and any test project that pulls in
`DecodeHost` will all resolve FFmpeg from that single folder — nothing
copies, nothing duplicates.

## Packaged / shipped builds
`VELNIX_FFMPEG_DIR` is a dev-machine convenience only — it must NOT be
relied on for anything the end user runs. The installer/packaging step
still needs to place the 5 DLLs in an `ffmpeg\` folder next to the final
installed `Velnix.exe`. That's a one-time packaging concern, not a
per-`dotnet build` concern, so it's intentionally not automated via
MSBuild here.

## Stale duplicates removed from this delivery
Previously-duplicated copies under `Velnix.App/bin/...` and
`tools/DecodeHarness/bin/...` are stale build output and were removed
when repackaging. They will not reappear on your next local build now
that the `CopyToOutputDirectory` item is gone — `bin`/`obj` should be
excluded from any zip going forward (`-x "*/bin/*" "*/obj/*"`).
