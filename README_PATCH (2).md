# Patch: wire MainWindow to the real Core Engine

## What this closes

Until now, `Velnix.App` had **zero real connection** to Phase 2's Core
Engine — this was newly discovered while reviewing the D3D11 renderer
work, not something flagged before:

- `Velnix.App.csproj` only referenced `Core` and `Rendering` (added
  earlier purely so `MainWindow.xaml.cs` could compile against
  `D3D11VideoRenderer` directly).
- `MainWindow.xaml.cs` constructed its own throwaway
  `new D3D11VideoRenderer()` instance, separate from whatever
  `CompositionRoot`/`PlaybackController` would use — so even once the
  decoder works, frames would never reach the screen because two
  different renderer instances would exist.
- `CompositionRoot.cs` still registered `NullVideoRenderer`, not the
  real `D3D11VideoRenderer`.
- The Play/Pause button only flipped a local `bool` and swapped an
  icon glyph — it never called anything in `Velnix.Playback`.
- There was no "Open file" UI at all — no way to ever call
  `IPlaybackController.OpenAsync`.

## Files changed

- `src/UI/CompositionRoot.cs` — registers `D3D11VideoRenderer` instead
  of `NullVideoRenderer` (Audio is still `NullAudioOutput` — WASAPI
  doesn't exist yet, unchanged).
- `Velnix.App/Velnix.App.csproj` — adds `ProjectReference`s to `UI`
  (which transitively pulls in Playback/DecodeHost/Rendering/Audio/etc.
  via its own references) and `Playback` directly.
- `Velnix.App/App.xaml.cs` — calls `CompositionRoot.Build()` once at
  startup, exposes it as `App.Services`, passes it into `MainWindow`'s
  constructor.
- `Velnix.App/MainWindow.xaml.cs` — resolves `IVideoRenderer` and
  `IPlaybackController` from DI instead of constructing its own
  renderer; adds a real "Open file" flow via `FileOpenPicker` that
  calls `OpenAsync` then `PlayAsync`; Play/Pause now calls
  `PlayAsync`/`PauseAsync` and the button's icon is driven by
  `PlaybackController.StateChanged` instead of local UI state, so it
  stays correct even if playback stops on its own (e.g. reaches Ended
  or Faulted) rather than only via the button.
- `Velnix.App/MainWindow.xaml` — adds the Open button next to the
  theme toggle in the title bar (there was previously no UI path to
  ever call `OpenAsync` at all).

## What's still NOT done, on purpose

- **Audio is still silent.** `NullAudioOutput` is still registered —
  no WASAPI implementation exists. Video should present (once the
  decoder itself is verified) but there's no sound.
- **`FFmpegNativeBootstrap.Initialize()` is not called anywhere.**
  Nothing points `FFmpeg.AutoGen` at the native DLL folder yet. Until
  that's wired into `CompositionRoot.Build()` (or somewhere early in
  `App.OnLaunched`), `OpenAsync` will very likely throw the first time
  it's called, even once the DLLs are in place.
- **Seek bar / position UI is not wired.** The transport bar's slider
  (if any) is untouched — this pass only covers Open + Play/Pause.
- **Mini-player does not share playback state yet.** It resolves its
  own things independently; not touched here.

## What's still NOT verified

This is, like everything else in this handoff, **unbuilt and unrun**.
It depends on:
1. `FFmpegMediaDecoder`/`PlaybackController` actually compiling clean
   against `FFmpeg.AutoGen` 8.1.0 (still unconfirmed — see
   `src/DecodeHost/DecodeHost_NOTES.md`).
2. `CompositionRoot.Build()` succeeding at all — if `FFmpegMediaDecoder`'s
   constructor (or anything in the DI graph) throws, the app won't launch.
3. `FileOpenPicker` + `WindowNative`/`InitializeWithWindow` interop
   working as documented for an unpackaged WinUI 3 app — this is a
   well-known pattern but has not been exercised in this codebase before.

## Suggested order once you're back on the Windows machine

1. `dotnet build 07_Source_Code\Velnix.sln` — fix whatever
   `FFmpeg.AutoGen` 8.1.0 API surface errors come up first (see prior
   patch's suggested next step).
2. Run `DecodeHarness` against a real file per
   `src/DecodeHost/DecodeHost_NOTES.md` — confirms the decoder alone
   works, independent of WinUI.
3. Wire `FFmpegNativeBootstrap.Initialize()` into `CompositionRoot.Build()`
   (one line, before the `IMediaDecoder` registration) so the DI-resolved
   decoder can actually find the native DLLs.
4. Build and run `Velnix.App`, click Open, pick an MP4, confirm video
   appears on `VideoSurface` (silent — no audio yet).
