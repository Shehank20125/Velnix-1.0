# Phase 4 Implementation Notes (handoff)

This pass implements Phase 4 (Subtitles, Audio, PiP, Screenshots, Bookmarks & Chapters)
against the Phase 1 module scaffold (`velnix-phase1-scaffold-PHASE4.zip`) and the Phase 3
WinUI 3 shell (`Velnix_Phase3_WinUI3-PHASE4.zip`), per
`Phase4_Playback_Features_Plan_AMENDED.md`.

**Important scope note:** this was built with no real C# Decode Host available. Phase 2's
actual delivered artifact is a separate, unconnected Qt/C++ prototype (`mpe_app` /
`mpe_decode_host`) that never links against this WinUI 3 C# app — it doesn't match the
Constitution's locked C#/WinUI 3 stack. Per the direction given for this pass, Phase 4 was
implemented against the Phase 1 interfaces with that gap left visible rather than
papered over. That decision is still open and should be resolved before Phase 5/6.

## Fully real and independently testable today

- **SRT/VTT parsing** (`SrtVttSubtitleParser`) — hardened against malformed timing,
  oversized files, and pathological cue counts. Unit-tested.
- **SSA/ASS parsing + rendering** — `AssSubtitleParser` extracts plain-text cues for the
  track menu; `AssSubtitleRasterizer` wraps libass (`Native/LibassNative.cs`, P/Invoke) for
  real styled bitmap rendering. Requires `ass.dll` alongside the app at runtime.
- **Subtitle resource limits** (`SubtitleResourceLimits`) — shared caps enforced across
  every parser/renderer per the Threat Model.
- **Subtitle track cataloging** (`SubtitleTrackCatalog`) — unifies embedded/external/
  downloaded tracks into one selectable list. Unit-tested.
- **Opt-in HTTPS subtitle downloading** (`SubtitleDownloadService`) — streams with a size
  cap, routes the result through the same untrusted-parsing path as any local file.
- **Audio track selection** (`AudioTrackSelector`). Unit-testable structure.
- **Audio output device enumeration/selection** (`WasapiAudioOutputDeviceService`) — real
  WinRT `Windows.Devices.Enumeration` calls.
- **Volume/mute persistence** (`AudioPreferencesService`) — Settings-backed. Unit-tested.
- **Screenshots** (`ScreenshotService` + `FrameCapturingVideoRenderer`) — real PNG encode
  via `System.Drawing.Common`, configurable save location, subtitle-inclusion toggle.
  Assumes an RGB24 raw frame buffer (matches Phase 2's documented pixel format — revisit if
  that changes).
- **Picture-in-Picture** (`PictureInPictureService`, `MiniPlayerWindow`) — real native
  Compact Overlay via `AppWindowPresenterKind.CompactOverlay`, with runtime verification and
  fallback to the existing always-on-top mini-player if it doesn't take. Feasibility spike
  resolved; see the plan doc's Amendment Log for details and the one real constraint found
  (fixed 16:9 aspect ratio in Compact Overlay mode).
- **Bookmark/chapter-cache contracts** (`IBookmarkStore`, `IChapterCache`) with in-memory
  placeholder implementations, matching the `InMemorySettingsStore` precedent. Unit-tested.

## Structurally defined, intentionally blocked pending a real Decode Host

These throw a recoverable `SubtitleParsingException`/`VelnixOperationException` or log-and-
return-empty rather than faking a result, so callers already degrade gracefully:

- **PGS/bitmap subtitle decode** — `DecodeHostBitmapSubtitleDecoder`.
- **Embedded chapter reading** — `DecodeHostChapterReader`.
- **Real PCM audio output** — `PreferenceBackedAudioOutput.WriteSamples`/`Flush` are no-ops.

## Before release

1. Resolve the Phase 2 stack mismatch (real C# Decode Host, or a different integration
   path) — this blocks closing out the three items above.
2. Replace `InMemoryBookmarkStore`/`InMemoryChapterCache` with Phase 5's SQLite-backed
   implementations once that schema lands; don't let these placeholders reach a release
   build.
3. Confirm `ass.dll` packaging/licensing (dynamic linking, per the Constitution) is wired
   into the build/CI pipeline.
4. Run the full Phase 4 testing plan from `Phase4_Playback_Features_Plan.md` against a real
   packaged build once a Decode Host exists — this pass's unit tests cover the
   decoder-independent logic only, not end-to-end playback.
