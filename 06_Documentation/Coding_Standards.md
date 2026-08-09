# Velnix Coding Standards

Phase: 1 — Foundation & Planning
Status: Draft for Phase 1 sign-off

## Language & Runtime

- C# on .NET (current LTS release targeted; re-evaluate at each major .NET LTS).
- Nullable reference types enabled project-wide — no suppressing warnings without a documented reason.
- `async`/`await` used consistently for I/O-bound work (file access, network calls, plugin IPC); avoid `async void` except for top-level event handlers.

## Project Structure

- One project per module where practical (Core, Playback, Rendering, Audio, Subtitles, Library, UI, Security, Network, Settings, Plugins, Diagnostics) — mirrors the Architecture Overview's module boundaries so dependency direction is enforced by the project reference graph itself, not just convention.
- UI project depends on the others; nothing depends "up" into UI (see Architecture_Overview.md).

## Dependency Injection

- Constructor injection by default; avoid service-locator patterns.
- Interfaces defined at module boundaries (e.g. `IMediaDecoder`, `ISubtitleRenderer`, `ILibraryStore`) so modules are testable in isolation and so the Plugin boundary has a clean interface to sandbox against.

## Naming & Style

- Standard .NET naming conventions (PascalCase for public members/types, camelCase for locals/private fields, `I`-prefixed interfaces).
- Use an `.editorconfig` checked into the repo so formatting is enforced by tooling, not code review nitpicking.
- Avoid abbreviations in public API names; clarity over brevity in anything another developer (or future-you) will read.

## Error Handling

- Never swallow exceptions silently — at minimum, log (respecting the opt-in Diagnostics policy) and fail gracefully.
- Media/subtitle parsing failures (per Threat Model) must be caught at the module boundary and surfaced as a recoverable UI state ("couldn't play this file"), never an unhandled crash.
- Distinguish between expected failures (bad file, network unavailable) and true bugs (should throw/log loudly in debug builds, fail gracefully in release).

## Testing

- Unit tests for Core, Playback (excluding actual FFmpeg calls — mock the decoder interface), Library, Security, and Settings logic.
- Integration tests covering the Playback → Rendering → Audio pipeline against real sample media files, including deliberately malformed ones (ties to Threat Model item 1).
- UI tests for critical user flows (open file, play/pause, seek, subtitle toggle).
- Performance tests that assert against the Constitution's numeric targets (cold start < 1s, idle RAM < 150MB) — these should run in CI, not just be checked manually before release.
- New code should not be merged without tests appropriate to its risk level; trivial UI tweaks don't need the same rigor as decode-path changes.

## Documentation

- Every public interface gets XML doc comments (`///`) — this also feeds IntelliSense for future-you and any collaborator.
- Module-level README in each project folder explaining its responsibility and its allowed dependencies, kept in sync with `Architecture_Overview.md`.

## Code Review (even solo)

- Even as a solo project, use pull requests against `main` rather than committing directly, so there's a record of what changed and why (feeds the "Okay-Great-Pack" progress reports and the AI Development Policy's review requirement for AI-generated code).
- AI-generated code is reviewed with the same scrutiny as any other contribution before merging — check license implications of any generated code/snippets, not just correctness.

## Dependencies

- No new dependency added without the Licensing & Linking Policy check (license compatibility with closed-source distribution) and a note in the relevant progress report's "Dependencies Added" section.
- Avoid duplication — if two modules seem to need similar logic, factor it into a shared, well-tested utility rather than copy-pasting.

## Commit & Branch Standards (carried from Constitution's Git Standards)

- Feature branches per unit of work.
- Commit messages describe *what* and *why*, not just *what* (e.g. "Fix subtitle timeout on malformed SSA files (Threat Model item 2)" rather than "fix bug").
- `main` is always releasable — no committing broken builds directly to `main`.
- Tag releases matching the Release Strategy stages (e.g. `v1.0.0-alpha.1`, `v1.0.0-beta.1`, `v1.0.0-rc.1`, `v1.0.0`).
