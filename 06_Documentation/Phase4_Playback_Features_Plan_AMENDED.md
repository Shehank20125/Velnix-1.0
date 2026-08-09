# Phase 4 — Playback Features: Planning Document

Status: Draft for Phase 4 sign-off
Depends on: Constitution v1.1, Architecture_Overview.md, Threat_Model.md, Phase2_Core_Engine_Plan.md, Phase3_UI_UX_Plan.md

## Goal (from roadmap)

Subtitles, audio, PiP, screenshots, bookmarks, chapters.

## Decisions Locked In

- **PiP is distinct from Phase 3's mini-player.** Phase 3 built an in-app compact/always-on-top window. Phase 4 adds a genuine **native Windows Compact Overlay / system PiP** experience on top of that — see risk note below.
- **Subtitle scope is broad:** SRT, VTT, SSA/ASS (text, via libass) **and** PGS/bitmap subtitles (image-based, via a separate compositing path — see Architecture_Overview.md and Threat_Model.md updates).
- **Subtitle downloading is in scope for Phase 4**, as an opt-in network feature per the Privacy Policy.

## Risk Flag: Native PiP / Compact Overlay Feasibility

`ApplicationView.TryEnterViewModeAsync(ApplicationViewMode.CompactOverlay)` is historically a **UWP** API tied to packaged apps. Whether — and how — an equivalent exists for a WinUI 3 / Windows App SDK desktop app (packaged or unpackaged) needs to be verified directly against current Windows App SDK documentation before committing to a design; this is not something to assume from general WinUI 3 familiarity, since packaging model (MSIX-packaged vs. unpackaged Win32) affects which APIs are actually available.

**Recommended approach:**
1. **Spike first, before full Phase 4 implementation begins:** a small throwaway prototype that attempts real Compact Overlay / PiP mode in Velnix's actual packaging configuration, to confirm feasibility, before the rest of Phase 4 is scheduled around it.
2. **Fallback if native Compact Overlay isn't available or isn't well-supported in this packaging model:** extend Phase 3's mini-player with PiP-like behavior — always-on-top by default when triggered, corner-snapping, staying visible over most windows — as a manually implemented approximation. This is a reasonable fallback since the mini-player already exists; it just wouldn't be the literal OS-level Compact Overlay surface (which, notably, can float above the taskbar and other apps system-wide in a way a normal always-on-top window sometimes can't fully replicate, e.g. across virtual desktops).
3. Document whichever path is actually taken in this file's Amendment Log once resolved — don't leave the ambiguity unresolved past the spike.

## Subtitles

- **Text-based (SRT/VTT/SSA/ASS):** rendered via libass, as already scoped in the Subtitles module. Style overrides (font, size, color, position) exposed to the user without breaking embedded SSA/ASS styling by default.
- **Bitmap-based (PGS):** decoded via FFmpeg (same Decode Host isolation as video — see Phase2_Core_Engine_Plan.md and the updated Threat Model), composited as an image overlay on the video frame. This is a genuinely different code path from libass text rendering — budget for it as such, not as a one-line "add another subtitle format" task.
- **External file support:** load external SRT/VTT/SSA/ASS files matched by filename convention or explicit user selection; PGS is realistically embedded-track-only (external standalone PGS files are uncommon) — confirm this assumption doesn't need revisiting once real-world test files are gathered.
- **Subtitle downloading:** opt-in, HTTPS-only, provider TBD (e.g. OpenSubtitles-style API) — provider choice, API terms of service, and rate limits are an open item for implementation; downloaded files are treated as untrusted input per the Threat Model update above, with no special trust extended based on provider reputation.
- **Track selection UI:** multiple subtitle tracks (embedded + external + downloaded) selectable from a single menu, consistent between main window and mini-player/PiP.

## Audio

- Multiple embedded audio track selection (e.g. multiple language tracks in a container).
- Output device selection (per the Audio module's existing scope) — respect the OS default output device by default, allow explicit override.
- Volume/mute persist as a Settings-module preference (local only, per Privacy Policy).
- No audio normalization/equalizer explicitly scoped here — flagging as an open item for a later phase (likely Phase 6 Advanced Features) rather than silently included or silently dropped.

## Picture-in-Picture

- See Risk Flag above for the core technical approach decision.
- Whichever implementation path is chosen, PiP must retain full playback control (play/pause/seek at minimum) and respect the mini-player's existing accessibility baseline from Phase 3 (keyboard operability, screen reader labels) — a PiP window is not exempt from the accessibility Definition of Done just because it's small.

## Screenshots

- Capture the current decoded video frame (post-render, so subtitle overlays are included only if the user explicitly wants a "with subtitles" option — otherwise capture the clean frame; expose this as a toggle rather than picking one silently).
- Format: PNG by default (lossless, universally supported).
- Save location: user-configurable, defaulting to a sensible local folder (e.g. Pictures/Velnix Screenshots) — no automatic cloud upload, consistent with local-first Privacy Policy.
- No metadata beyond the image itself is embedded (no EXIF location/device data — there's nothing sensitive to strip since Velnix doesn't have geolocation, but confirm the chosen imaging library doesn't add anything unexpected).

## Bookmarks & Chapters

- **Chapters:** read from embedded container metadata (MP4/MKV chapter markers) where present; exposed as a navigation menu (jump to chapter).
- **Bookmarks:** user-created markers at arbitrary timestamps, stored locally (Library/Settings module — cross-reference with Phase 5, since persistent storage of "things about media" is nominally Phase 5's Library module territory; Phase 4 defines the *feature*, Phase 5 owns the *storage schema*, and these should be coordinated rather than Phase 4 inventing its own ad hoc local storage that Phase 5 then has to reconcile).
- No cloud sync of bookmarks/chapters — local-only, consistent with Privacy Policy, unless a future v3.x cloud feature is formally proposed (which would require its own constitutional amendment per the AI Feature Policy's precedent for privacy-affecting features).

## Testing Plan (Phase 4 specific)

- Subtitle rendering tests across all supported formats, including deliberately malformed files for each (text-based malformed SSA per existing Threat Model coverage; malformed/truncated PGS streams as a new case).
- Subtitle downloading: test opt-in flow, HTTPS enforcement, and that downloaded files are routed through the same validation as local files (no special-casing that weakens scrutiny).
- PiP/Compact Overlay: test across the packaging model actually used, plus the fallback path if native Compact Overlay proves infeasible.
- Screenshot tests: format correctness, save-location handling, subtitle-inclusion toggle behavior.
- Bookmark/chapter tests: embedded chapter parsing across container formats, bookmark persistence and retrieval.

## Acceptance Criteria

- [ ] Native PiP feasibility spike completed and documented; implementation path (native Compact Overlay or mini-player-based fallback) decided and recorded in this file's Amendment Log.
- [ ] Text-based subtitles (SRT/VTT/SSA/ASS) render correctly via libass, with style overrides available.
- [ ] PGS/bitmap subtitles render correctly via the separate compositing path, contained by the same Decode Host isolation as video.
- [ ] Subtitle downloading works as an opt-in, HTTPS-only feature; downloaded files are treated with the same scrutiny as local files.
- [ ] Multi-track audio selection and output device selection work correctly.
- [ ] PiP retains playback controls and meets the Phase 3 accessibility baseline.
- [ ] Screenshots save correctly in PNG, with a working subtitle-inclusion toggle and configurable save location.
- [ ] Embedded chapters parse and display as a navigation menu; user bookmarks persist locally and coordinate with Phase 5's Library storage schema rather than duplicating it.

## Amendment Log

- **PiP feasibility spike: RESOLVED.** The Risk Flag's core worry -- that
  `ApplicationView.TryEnterViewModeAsync(ApplicationViewMode.CompactOverlay)` is a UWP-only
  API -- is correct for that specific API, but does not block this app: Windows App SDK's
  `AppWindow`/`OverlappedPresenter` family exposes the same capability as
  `AppWindowPresenterKind.CompactOverlay` via `AppWindow.SetPresenter`, which is documented
  as usable by any Windows App SDK desktop app (WinUI, WPF, WinForms, Win32; packaged or
  unpackaged) -- not tied to the UWP app model. Velnix's current MSIX-packaged WinUI 3
  configuration (`Velnix.csproj`) is well within that support surface.
  **Decision: native Compact Overlay is the primary PiP path**, implemented in
  `Services/PictureInPictureService.cs` and applied to `Views/MiniPlayerWindow.xaml.cs` via
  `EnterPictureInPicture()`. One real constraint surfaced by the spike: `CompactOverlay`
  forces a small, fixed 16:9-aspect window (not freely resizable like the existing
  mini-player) -- an acceptable trade-off for genuine system-level PiP (floats above the
  taskbar/other apps/virtual desktops the way a normal always-on-top window can't fully
  replicate), but worth calling out explicitly since it means PiP and the plain resizable
  mini-player remain two distinct experiences rather than one subsuming the other.
  **Fallback path implemented, not just planned:** if `AppWindow.SetPresenter` doesn't
  actually take (verified by checking `AppWindow.Presenter.Kind` afterward, since the call
  can silently no-op rather than throw), `PictureInPictureService`/`MiniPlayerWindow` fall
  back to the mini-player's existing always-on-top, corner-snapping, borderless
  `OverlappedPresenter` configuration -- which already *is* the fallback design described
  above, so no separate fallback implementation was needed beyond wiring the fallback path
  to trigger correctly.
  Both paths retain full playback controls and Phase 3's accessibility baseline
  (`AutomationProperties`, keyboard accelerators), per this document's Picture-in-Picture
  section and Acceptance Criteria.

- **Scope note (not a plan change, a delivery note):** this Phase 4 pass was implemented
  against the Phase 1 module interfaces and Phase 3's WinUI 3 shell, without a real C#
  Decode Host available (Phase 2's actual delivered artifact is an unconnected Qt/C++
  prototype, not the C#/WinUI 3 stack the Constitution and Architecture_Overview.md lock
  in). Concretely: text-based subtitle parsing (SRT/VTT/SSA/ASS via libass),
  resource-limited rendering, subtitle track cataloging, and opt-in HTTPS downloading are
  fully implemented and don't depend on a decoder. PGS/bitmap subtitle decode, embedded
  chapter reading, and real PCM audio output are structurally in place
  (`IBitmapSubtitleDecoder`, `IChapterReader`, `IAudioOutput`) but intentionally throw/no-op
  with a clear "requires the Decode Host" message rather than faking a result, since no
  real decoder exists yet to source that data from. These three are the acceptance-criteria
  items that cannot be closed out until a real C# Decode Host lands.
