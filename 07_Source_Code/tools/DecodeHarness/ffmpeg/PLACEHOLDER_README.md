# ffmpeg/ — placeholder, not the real thing

This folder is a placeholder created ahead of a build. It is empty.

## What goes here

The 5 LGPL FFmpeg shared-build DLLs:
- avutil-*.dll
- avcodec-*.dll
- avformat-*.dll
- swscale-*.dll
- swresample-*.dll

Source: gyan.dev/ffmpeg/builds, "shared" build (NOT "static"), **FFmpeg 8.1.x ("Hoare")**
specifically — must match the `FFmpeg.AutoGen` 8.1.0 package pinned in
`src/DecodeHost/DecodeHost.csproj` (this is the newest version actually published on
NuGet — `9.0` doesn't exist there yet despite matching FFmpeg's own release numbering).
Using a 7.x or 9.x DLL set here will fail at the first native call — sonames don't match.

## Where they actually need to live at runtime

Not here. After you build DecodeHarness in Visual Studio, MSBuild creates
a bin/ output folder, e.g.:

    tools\DecodeHarness\bin\x64\Debug\net8.0-windows10.0.19041.0\

Create an "ffmpeg\" subfolder *inside that build output folder* and put
the 5 DLLs there — that's where FFmpegNativeBootstrap.cs (via
AppContext.BaseDirectory) actually looks at runtime.

This folder (tools\DecodeHarness\ffmpeg\) is just staging — safe to
delete once you've copied the DLLs to the real output location, or keep
it as your source-of-truth copy if you want one .gitignore'd staging spot.
