# Threat Model

**Status per Phase 1 tracking:** DONE (ratified during Phase 1).

> **Note on this version:** the originally-ratified Phase 1 Threat Model
> text was not part of the file set available when this repository was
> scaffolded. This document has been drafted to fill that gap, expanding
> on the four attack surfaces already named (and ranked as equal
> priority) in `Velnix_Project_Constitution_v1_1.md` § Threat Model &
> Security Requirements. If the original ratified document exists
> elsewhere, replace this file with it.

Velnix treats the following four attack surfaces as equally important
from Phase 1 onward — none is deferred behind the others.

## 1. Malicious Media Files

**Surface:** Any media container or stream opened by the user — local
file, network stream, or (in the plugin era) a plugin-supplied path —
passed to FFmpeg via `Playback.IMediaDecoder`.

**Risk:** Malformed or adversarial containers/streams are a classic
memory-corruption and code-execution vector against media decoders.

**Mitigations:**
- FFmpeg is treated as hostile-input-facing by default; all calls into
  it are isolated behind `IMediaDecoder` so this boundary is the single
  place hardening effort concentrates.
- Parsing/decode failures must surface as `Core.Exceptions.
  MediaParsingException` (`IsRecoverable = true`), caught at the module
  boundary and shown as "couldn't play this file" — never an unhandled
  crash, per Coding_Standards.md § Error Handling.
- Integration tests must include deliberately malformed sample media
  files, not just well-formed ones (Coding_Standards.md § Testing).
- FFmpeg is dynamically linked (Licensing & Linking Policy), which also
  means a future FFmpeg security patch can ship without a full app
  rebuild.

## 2. Malicious Subtitle Files

**Surface:** Subtitle files (SRT, SSA/ASS) parsed by `Subtitles.
ISubtitleParser` and rendered by `ISubtitleRenderer` via libass.

**Risk:** SSA/ASS in particular supports embedded scripting/styling
directives, historically a real-world attack vector in media players.
Untrusted subtitle files are frequently downloaded from third-party
sources by users, so this surface is attacker-reachable in practice, not
just in theory.

**Mitigations:**
- Parsing (`ISubtitleParser`) and rendering (`ISubtitleRenderer`) are
  separate interfaces specifically so the security-sensitive parse path
  can be scrutinized and tested independently of rendering performance
  work.
- No subtitle-triggered code execution or unbounded resource use is
  acceptable, full stop — a malformed or adversarial subtitle file must
  produce `Core.Exceptions.SubtitleParsingException`
  (`IsRecoverable = true`), never execute embedded logic as code.
- Treated with the same suspicion as media containers (item 1) rather
  than as a "just text" format.

## 3. Malicious or Misbehaving Plugins

**Surface:** Third-party (and first-party) plugins loaded via
`Plugins.IPluginHost`, executed through `Security.IPluginSandbox`.

**Risk:** Plugins are, by definition, code the project owner does not
fully control (explicitly in scope starting Phase 6 per the
Constitution's Long-Term Roadmap). A misbehaving or malicious plugin
could otherwise access host memory, the filesystem, or the network
without limit.

**Mitigations (see Constitution § Plugin Security Model for full detail):**
- Full process isolation — each plugin runs in its own OS process, never
  in-process with the main application.
- Communication only through a defined, versioned API/IPC boundary — no
  direct memory access.
- Explicit, opt-in capabilities (`Velnix.Security.PluginCapability`:
  `ReadMediaMetadata`, `NetworkAccess`, `FileSystemReadAccess`) granted
  per plugin at launch, never ambient or all-or-nothing.
- A crashing or hanging plugin must not be able to crash or hang the
  main Velnix process — this is a hard requirement on
  `IPluginSandbox.LaunchAsync`'s eventual implementation, not a
  best-effort goal.
- Plugins are treated as untrusted even when first-party, so the sandbox
  is never bypassed "just this once" for convenience.

## 4. Supply Chain / Dependency Compromise

**Surface:** Every third-party dependency Velnix takes on (NuGet
packages, FFmpeg, libass), plus the build/release pipeline itself.

**Risk:** A compromised or maliciously-updated dependency, or a
tampered build/release pipeline, can inject arbitrary code without the
media/subtitle/plugin surfaces ever being touched.

**Mitigations:**
- Every dependency (including transitive ones) is reviewed before
  adoption for both license compatibility (Licensing & Linking Policy)
  and provenance/trustworthiness — no dependency is added without this
  check, regardless of how useful it is (Coding_Standards.md §
  Dependencies).
- Dependency updates are reviewed, not auto-merged blindly.
- Build/release pipelines are protected against tampering: signed
  commits/tags where practical, no unreviewed third-party CI actions
  granted write access to secrets.
- `main` is always releasable and protected by PR review even on a solo
  project, so there's always a record of what changed and why
  (Coding_Standards.md § Code Review).
- The Source Code Policy's absolute "never published, mirrored, or
  open-sourced" rule also reduces the attack surface for a supply-chain
  actor trying to plant a malicious PR against a public mirror — there
  isn't one.

## Cross-Cutting Requirements (carried from the Constitution)

Secure by Design; validate all inputs (see `Security.IInputValidator`);
sandbox media processing where practical; HTTPS-only networking (see
`Network.ISecureHttpClient`, which must reject non-HTTPS URLs outright);
secure updates (signed packages); dependency audits; privacy-first
defaults; secure plugins.
