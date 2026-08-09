# Velnix.Diagnostics

**Status:** Scaffolded — implementation pending (Phase 2+).

## Responsibility

_TODO: one-paragraph description of what this module owns, sourced from
04_Architecture/Architecture_Overview.md when that document is transferred
into this repository._

## Allowed Dependencies

Core

Per Coding_Standards.md, this module's project references must never violate
the dependency direction defined in Architecture_Overview.md — the project
reference graph is the enforcement mechanism, not just convention.

## Testing

See `tests/Diagnostics.Tests`. Per Coding_Standards.md:
- Unit tests required for Core, Playback (excluding real FFmpeg calls — mock
  the decoder interface), Library, Security, and Settings logic.
- Other modules follow the risk-appropriate testing guidance in
  Coding_Standards.md § Testing.
