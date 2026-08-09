# Architecture Overview

**Status per Phase 1 tracking:** DONE (ratified during Phase 1).

> **Note on this version:** the originally-ratified Phase 1 Architecture
> Overview text was not part of the file set available when this
> repository was scaffolded. This document has been drafted to fill that
> gap, derived from the module list, dependency direction, and boundary
> requirements already fixed by `Velnix_Project_Constitution_v1_1.md`,
> `Coding_Standards.md`, and the module `README.md` files (and matching
> the actual `ProjectReference` graph in each `.csproj`). If the original
> ratified document exists elsewhere, replace this file with it and treat
> this draft only as a fallback that keeps the repo internally consistent
> in the meantime.

## Module List & Responsibilities

| Module | Responsibility |
|---|---|
| **Core** | Shared types with no dependencies on any other Velnix module: domain models (`MediaInfo`, `PlaybackPosition`), shared enums (`PlaybackState`), and the `VelnixException` hierarchy. Exists so two modules never need to depend on each other just to share a type. |
| **Playback** | Owns the FFmpeg-facing decode path behind `IMediaDecoder`, and playback orchestration (open/play/pause/seek/stop, state machine) behind `IPlaybackController`. Treats all media input as hostile per the Threat Model. |
| **Rendering** | Presents decoded video frames to a UI-provided surface (`IVideoRenderer`). Depends on Playback so it can consume decoded frame data, but is developed/tested against fakes rather than a real decoder. |
| **Audio** | Audio output (`IAudioOutput`): volume, mute, sample writing, flush. Kept separate from Rendering so audio/video sync logic in Playback can depend on each independently. |
| **Subtitles** | Subtitle parsing (`ISubtitleParser`, SRT/SSA/ASS via libass) and rendering (`ISubtitleRenderer`), deliberately split into two interfaces so the security-sensitive parse path and the performance-sensitive render path can be tested and optimized independently. |
| **Library** | Local media library index (`ILibraryStore`): add/remove/list entries, last-played position. Local-only per the Privacy Policy — no implementation may transmit library contents off-device. |
| **Security** | Cross-cutting security boundary: input validation (`IInputValidator`), the plugin sandbox (`IPluginSandbox`, full process isolation), and the `PluginCapability` enum it grants at launch. Has no dependency on Plugins, so Plugins can safely depend on Security without a cycle. |
| **Network** | The single boundary for outbound network calls (`ISecureHttpClient`), HTTPS-only. AI features must never call through this module (AI Feature Policy: local-only). |
| **Settings** | User settings storage (`ISettingsStore`), local-file-backed in the eventual real implementation (currently an in-memory placeholder, `InMemorySettingsStore`, for testing and DI wiring). |
| **Plugins** | Plugin discovery/lifecycle (`IPluginHost`) and the plugin contract (`IPlugin`). Delegates actual process isolation to Security's `IPluginSandbox` rather than launching untrusted code itself. |
| **Diagnostics** | Opt-in-only logging (`IVelnixLogger`), defaulting to a no-op (`NullVelnixLogger`) until the user explicitly opts in, per the Privacy Policy. |
| **UI** | WinUI 3 application shell and composition root (`CompositionRoot`). The only module allowed to know about concrete implementations across other modules; every other module depends only on interfaces. |

## Dependency Direction

Enforced by the project reference graph itself (`ProjectReference` in
each `.csproj`), not just by convention, per Coding_Standards.md:

```
Core                     (no dependencies — foundation)
├── Playback              → Core
├── Library                → Core
├── Settings               → Core
├── Diagnostics             → Core
├── Security                → Core
├── Rendering                → Core, Playback
├── Audio                     → Core, Playback
├── Subtitles                  → Core, Playback
├── Network                     → Core, Security
├── Plugins                      → Core, Security
└── UI                            → all of the above (nothing depends up into UI)
```

Rules that follow from this graph:

- **Core has zero Velnix dependencies.** Any type two or more modules
  need to share (enums, exceptions, value types) generally belongs in
  Core — but the rule that actually matters is *no circular project
  reference*, not "always Core." `PluginCapability` is the example: it
  lives in **Security** (`Velnix.Security.PluginCapability`), not
  Plugins, because `IPluginSandbox` (Security) needs it and
  `Plugins.csproj` already references `Security.csproj` — so Security →
  Plugins would be the cycle to avoid, and putting the type in Security
  itself (the side already depended upon) sidesteps that cleanly without
  even needing Core involved.
- **Security does not depend on Plugins.** Plugins depends on Security
  (to launch sandboxed processes), so the reverse dependency is
  forbidden — it would create a cycle.
- **UI is a dependency sink.** Every other module can be built and
  tested with zero knowledge that UI exists. UI is the only place
  concrete implementations are wired together (`CompositionRoot`).
- **Rendering, Audio, and Subtitles depend on Playback**, not on each
  other, and not on UI directly — Playback's `IPlaybackController` is
  the single surface UI talks to for orchestration; UI does not reach
  into Rendering/Audio/Subtitles independently for playback control.

## FFmpeg / libass Integration Boundary

- All FFmpeg interaction is confined to **Playback**, behind
  `IMediaDecoder`. No other module calls into FFmpeg directly — Rendering
  and Audio consume already-decoded frame/sample data, not raw FFmpeg
  handles.
- Per the Licensing & Linking Policy, FFmpeg is an **LGPL-configured,
  dynamically linked** shared library loaded at runtime — never statically
  linked into the Velnix binary. `IMediaDecoder` is the seam that makes
  this swap possible without touching calling code.
- libass integration lives behind **Subtitles**' `ISubtitleRenderer` for
  compositing, with parsing (`ISubtitleParser`) kept as a separate,
  more heavily scrutinized interface per the Threat Model (subtitle
  formats — especially SSA/ASS scripting/styling — are a known attack
  vector).
- Both boundaries throw Core's recoverable exception types
  (`MediaParsingException`, `SubtitleParsingException`) on hostile or
  malformed input rather than allowing a crash or unbounded resource use,
  per the Threat Model and Coding_Standards.md's error-handling rules.

## Plugin Boundary

- Plugins never run in-process. **Plugins** (lifecycle/discovery) and
  **Security** (`IPluginSandbox`, actual process isolation) are
  deliberately separate modules so that "which plugins exist" and "how a
  plugin is safely executed" can be reasoned about and tested
  independently.
- The only communication path between host and plugin is the versioned
  API/IPC boundary described in the Constitution's Plugin Security Model
  — no direct memory access, no ambient filesystem/network access.
- Capabilities (`Velnix.Security.PluginCapability`, defined on the
  Security side since Plugins already depends on Security) are explicit
  and opt-in per plugin, granted by the sandbox at launch time, never
  assumed.
- This boundary is scaffolded now (Phase 1) but not implemented — real
  process-isolation logic is Phase 6 scope per the Constitution's
  Long-Term Roadmap.
