# Velnix — Consolidated Build (read this first)

Your uploads contained **three separate, mutually incompatible prototypes** built at
different times: a C#/WinUI 3 line, a Qt + libmpv C++ line, and an FFmpeg + IPC decode-host
C++ line. Only one of them matches what `Velnix_Project_Constitution_v1_1.md` actually
locks in:

> **Technology Stack:** C# (.NET) · WinUI 3 · FFmpeg (LGPL) · libass · **Visual Studio
> Community**

That's also confirmed independently by your own `PHASE4_IMPLEMENTATION_NOTES.md`, which
flags the Qt/C++ decode host as *"a separate, unconnected Qt/C++ prototype... [that]
doesn't match the Constitution's locked C#/WinUI 3 stack."*

So this folder merges every C#/WinUI 3 piece that was actually found across your uploads
into **one solution**, and moves the two non-conforming C++ prototypes into
`08_Archive_NonConforming_Prototypes/` — kept for reference, excluded from the solution.

## What's in `07_Source_Code/Velnix.sln`

| Project | Source | Phase |
|---|---|---|
| `src/Core`, `Playback`, `Rendering`, `Audio`, `Subtitles`, `Library`, `UI`, `Security`, `Network`, `Settings`, `Plugins`, `Diagnostics` (+ matching `tests/*`) | `velnix-scaffold` | Phase 1 — module scaffold, interfaces only |
| `Velnix.App` | `Velnix/Velnix` (Phase 3 shell) with the Phase‑4 update layered on top (adds `PictureInPictureService`, etc.) | Phase 3 + partial Phase 4 |
| `Velnix.MediaLibrary` | standalone SQLite-backed library project | Phase 5 (built ahead of schedule, **not yet wired** into `src/Library`) |

26 projects total, all referenced in `Velnix.sln`.

## The real gap — read before doing anything else

`PHASE4_IMPLEMENTATION_NOTES.md` (in `06_Documentation/`) says it plainly: **Phase 2 (the
real Core Engine / Decode Host) was never actually built in C#.** The only Phase 2 work
that exists is the two archived C++ prototypes, which don't count because they don't link
against the C# app at all. Everything downstream — real subtitle bitmap decode, real PCM
audio output, embedded chapter reading — is stubbed out waiting on this.

Also worth noting: the actual Phase 4 C# source files described in that handoff note
(`SrtVttSubtitleParser`, `AssSubtitleRasterizer`, `WasapiAudioOutputDeviceService`, etc.)
were **not present in any of the zips you uploaded** — only the planning doc and the shell
update (`PictureInPictureService`) made it in. If that code exists somewhere else, it's
worth tracking down before redoing the work.

## Before you open this in Visual Studio

This was assembled without a .NET SDK available, so it has **not** been opened in Visual
Studio or run through `dotnet build` yet — same caveat the scaffold's own
`Velnix.sln.README.md` already carries. First moves on a Windows machine with VS 2022 +
".NET Desktop Development" + "Windows App SDK" workloads installed:

```
dotnet sln 07_Source_Code\Velnix.sln list     # confirms it parses, lists all 26 projects
dotnet build 07_Source_Code\Velnix.sln        # first real compile
dotnet test 07_Source_Code\Velnix.sln         # runs existing unit tests
```

If the solution file itself errors (not project code), regenerate it per the instructions
in `07_Source_Code\Velnix.sln.README.md` rather than hand-patching further.

## Update: Phase 2 Core Engine has a real first implementation

`src/DecodeHost/` now contains `FFmpegMediaDecoder` — a real `IMediaDecoder`
implementation via FFmpeg.AutoGen (LGPL, dynamically linked), plus `src/Playback/
PlaybackController.cs` wiring decode → render/audio with PTS-based A/V sync. Wired into
`UI/CompositionRoot.cs`. 27 projects now in `Velnix.sln`.

**This has not been compiled or run anywhere** — there's no Windows/.NET/FFmpeg
environment available to test against in this sandbox. Read `src/DecodeHost/NOTES.md`
before trusting it; it lists exactly what's unverified and what to check first.

Two pieces were deliberately left as placeholders rather than rushed alongside the decode
path, because they're each their own substantial native-interop surface: `NullVideoRenderer`
(needs real WinUI/D3D11 presentation) and `NullAudioOutput` (needs real WASAPI output).
Both follow the same swap-later pattern already used elsewhere in the scaffold
(`NullVelnixLogger`, `InMemorySettingsStore`) — see their doc-comments for what's next.
