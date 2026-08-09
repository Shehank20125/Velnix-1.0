# D3D11/WinUI Video Presentation — Implementation Notes (handoff)

This pass implements a real `Velnix.Rendering.IVideoRenderer` — a D3D11 device
presenting into a DXGI composition swapchain bound to a WinUI 3 `SwapChainPanel` —
against the Phase 1 `IVideoRenderer` contract and the Phase 3 WinUI 3 shell
(`MainWindow.xaml`).

**Scope note, same spirit as PHASE4_IMPLEMENTATION_NOTES.md:** this only wires up
enough of `Velnix.App` for the surface itself to attach, resize, and be ready to
receive frames. It does **not** wire `Velnix.UI.CompositionRoot`, `IPlaybackController`,
or the decoder into `MainWindow` — that graph still doesn't reach `Velnix.App` at all.
Found, not fixed: **`Velnix.App.csproj` had zero `ProjectReference`s before this pass.**
Added only `Core` and `Rendering`, i.e. exactly what `D3D11VideoRenderer` needs to
compile and run. `PresentFrame` is therefore not reachable from anywhere yet — the
panel will show solid black (its `Background`) behind the existing "No media loaded"
overlay until the next pass wires `CompositionRoot` → `IPlaybackController` into this
window.

## Fully real today

- **`D3D11VideoRenderer`** (`src/Rendering/D3D11VideoRenderer.cs`) — D3D11 device
  (hardware adapter, WARP software fallback), DXGI `CreateSwapChainForComposition`,
  bound to the panel via `Vortice.WinUI.ISwapChainPanelNative`'s `nint` constructor
  (no compile-time WinUI/XAML dependency in `Rendering.csproj` — see its doc-comment).
  Per-frame: RGB24→BGRA32 convert, upload into a `Usage.Dynamic` texture
  (`Map`/`WriteDiscard`, respects `RowPitch` for driver row padding), draw a
  `SV_VertexID` fullscreen triangle sampling that texture, `Present`.
- **`PixelFormatConverter.Rgb24ToBgra32`** (`src/Rendering/PixelFormatConverter.cs`) —
  the one piece of this that's meaningfully unit-testable without a GPU. Unit-tested
  (`tests/Rendering.Tests/PixelFormatConverterTests.cs`, 6 cases).
- **`IVideoRenderer.Resize` + `IDisposable`** — extended onto the interface;
  `NullVideoRenderer` updated to match, still available for GPU-less tests/CI.
- **`RenderingException`** (`src/Core/Exceptions/RenderingException.cs`) — follows the
  existing `VelnixException`/`MediaParsingException` pattern; marked recoverable.
- **`MainWindow` wiring** — `SwapChainPanel` (`VideoSurface`) added to
  `MainWindow.xaml` under the existing "No media loaded" overlay. Code-behind
  constructs `D3D11VideoRenderer`, attaches on `Loaded`, resizes on `SizeChanged`/
  `CompositionScaleChanged` (DPI-aware — multiplies by `CompositionScaleX/Y`), disposes
  on window close. Falls back to leaving the overlay as the only visible state (no
  crash) if device/swapchain creation throws `RenderingException`.

## Structurally defined, intentionally not scoped here

Documented in `D3D11VideoRenderer`'s class doc-comment rather than silently shipped:

- **Aspect-ratio letterboxing** — the fullscreen triangle stretches to fill the whole
  panel; non-panel-aspect video will look distorted until this computes a
  viewport/quad from the frame's aspect ratio instead.
- **Zero-copy / hardware decode path** — frames arrive as CPU-side packed RGB24 and
  are converted + uploaded every frame. Fine for a first working path; not fine for
  4K/high-fps without moving the conversion into the decoder's swscale step (upload
  BGRA directly) or a YUV shader path.
- **HDR / wide color / 10-bit** — fixed at `DXGI_FORMAT_B8G8R8A8_UNORM` (8-bit SDR).
- **DXGI device-removed recovery** — a driver crash/GPU reset currently surfaces as a
  `RenderingException` out of the next `PresentFrame` rather than rebuilding the
  device transparently.

## Not independently verified — needs `dotnet build` on your machine

No .NET SDK in this sandbox, so none of this has actually compiled. Everything above
is written against documented Vortice.Windows API shapes and real sample code, not
against your exact installed package version. Two spots flagged in-code as most likely
to need a one-line fix (same category as the `FFmpeg.AutoGen` signature drift already
hit in `DecodeHarness`):

1. **`Vortice.D3DCompiler.Compiler.Compile(...)`** in `D3D11VideoRenderer.CompileShaders`
   — the overload shape (return-`Blob` vs. `out Blob`/`out errorBlob`) has shifted across
   package versions. Flagged at the call site.
2. **`D3D11.D3D11CreateDevice(...)`** — used the 5-out-param overload
   (`device, context`, no `out FeatureLevel`), confirmed against a real Vortice sample,
   but worth a second look if it doesn't resolve.

Also unverified: exact `Vortice.WinUI` package version that ships the `nint`-based
`ISwapChainPanelNative` constructor (used deliberately to avoid a WinUI/XAML
dependency in `Rendering.csproj` — see AttachSurface's doc-comment). Pin a version and
confirm this constructor exists before relying on it.

## Before this is presentation-complete

1. Wire `CompositionRoot` → `IPlaybackController` into `MainWindow` so `PresentFrame`
   is actually reachable — otherwise everything above is plumbing with nothing
   flowing through it.
2. `dotnet build` / `dotnet test` locally; fix whatever the two flagged API spots
   turn out to need.
3. Aspect-ratio letterboxing before this ships — full-stretch will visibly distort
   almost all real video.
4. Confirm `Vortice.*` package licensing (MIT) is captured wherever the project
   tracks third-party dependency licenses, per the Commercial Policy's "review all
   dependencies before adoption."
