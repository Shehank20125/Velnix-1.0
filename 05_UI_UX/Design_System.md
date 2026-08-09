# Design System — Phase 1 Starting Point

**Status per Phase 1 tracking:** DONE - DRAFT. Accent color and exact
type scale are deliberately deferred to Phase 3, once real screens exist
to validate against — this document establishes principles and
structure, not final pixel values.

> **Note on this version:** the originally-ratified Phase 1 Design
> System draft text was not part of the file set available when this
> repository was scaffolded. This document has been drafted to fill that
> gap, derived from the UI/UX Principles and Core Principles already
> fixed by `Velnix_Project_Constitution_v1_1.md`. If the original
> ratified draft exists elsewhere, replace this file with it.

## Guiding Principles

Per the Constitution's UI/UX Principles: modern, minimal, consistent,
accessible, responsive, intuitive, with elegant animations. Per Core
Principles, User Experience and Performance are both "first" — an
animation or visual flourish that costs cold-start time or idle RAM
against the Performance Targets is not acceptable as-is; it must be
optimized or made optional.

## Color

- **Deferred to Phase 3:** exact accent color and full palette. Choosing
  now, before any real screen exists to validate contrast and mood
  against, risks a palette that looks fine in isolation but fights the
  actual UI.
- **Fixed now:** the palette must support both a light and dark theme
  (Windows convention, WinUI 3 native support), must meet WCAG AA
  contrast minimums for text/background pairs, and must not rely on
  color alone to convey state (e.g. playback error vs. playing) —
  pair color with icon/shape/text per the Accessibility Baseline below.

## Typography

- **Deferred to Phase 3:** exact type scale (specific point sizes per
  heading level, line-height values).
- **Fixed now:** use the WinUI 3 default type ramp (Segoe UI Variable)
  as the starting point rather than a custom font, to keep native-feel
  consistency with Windows and avoid a custom-font performance/licensing
  review this early. A custom scale can be layered on top once real
  screens exist to test it against, in Phase 3.

## Iconography

- Use Fluent-style iconography (consistent with WinUI 3 / Windows 11
  conventions) rather than a custom icon language, for the same
  native-feel-consistency reason as typography.
- Icons conveying state (play/pause/buffering/error) must never be the
  sole indicator — always paired with a text label or tooltip
  (Accessibility Baseline).

## Motion

- Animations should clarify state changes (e.g. a control bar fading in
  on hover, a subtitle track toggling) rather than decorate for its own
  sake, per "elegant animations" in the Constitution.
- Respect the OS-level "reduce motion" accessibility setting — when set,
  animations must be disabled or reduced to instant/near-instant
  transitions, not just shortened.
- Any animation must be evaluated against the Performance Targets same
  as any other feature — a smooth-looking animation that regresses cold
  start or idle RAM is a Definition-of-Done failure, not a UX win.

## Accessibility Baseline

- Full keyboard navigation for all playback controls (play/pause, seek,
  volume, subtitle toggle) — mouse/touch must never be the only path to
  a control.
- Screen reader labels (WinUI 3 `AutomationProperties`) on every
  interactive control, especially icon-only buttons.
- WCAG AA minimum contrast for all text/background and icon/background
  pairs, checked against whatever palette Phase 3 lands on — this
  baseline doesn't change even though the palette itself is deferred.
- State conveyed by more than color alone (see Color section above).
- Respect OS text-scaling settings rather than fixing pixel sizes that
  ignore user zoom/scale preferences.

## Layout Principles

- Content (the video surface) is the primary element; chrome (controls,
  library browser, settings) recedes and is dismissible/collapsible
  rather than permanently competing for space, consistent with "minimal."
- Responsive to window resize down to a reasonable minimum window size —
  no fixed-pixel-only layouts that break on a smaller display or a
  non-maximized window.
- Consistent spacing/alignment grid across screens once real screens
  exist (Phase 3+), rather than each screen inventing its own spacing
  ad hoc.
