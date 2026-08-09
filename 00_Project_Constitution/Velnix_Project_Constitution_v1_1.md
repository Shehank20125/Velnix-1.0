# VELNIX PROJECT CONSTITUTION

Version 1.1
Supersedes: Version 1.0
Amendment Date: 2026-08-03

> Per the Constitutional Rule of v1.0, no change may violate the constitution unless it is formally amended and versioned. This document is that amendment. All sections not explicitly changed below carry forward unchanged from v1.0.

## Mission

Velnix exists to become a modern, efficient, secure, and beautiful
desktop media player. Every feature must improve performance, usability,
security, or reliability.

## Vision

Build a premium desktop media player focused on performance, security,
maintainability, accessibility, and outstanding user experience.

## Core Principles

- Performance First
- Security First
- User Experience First
- Stability First
- Maintainability
- Privacy by Default

## Technology Stack

- C# (.NET)
- WinUI 3
- FFmpeg (LGPL build only — see Licensing & Linking Policy)
- libass
- Visual Studio Community
- Git & GitHub (private repository — see Source Code Policy)
- Netlify (public website only, never application source)
- Markdown documentation

## Platform Scope *(NEW in v1.1)*

- **v1.x:** Windows-only. WinUI 3 remains the UI stack; no cross-platform abstraction work is required or expected in v1.x.
- **v2.x / v3.x:** Cross-platform (Linux/macOS) may be considered. Any future cross-platform effort must be scoped as its own architecture decision — it is not assumed or partially built into v1.x.
- Until a cross-platform decision is formally made, no code should introduce artificial cross-platform abstraction layers "just in case." Build for Windows/WinUI 3 directly; abstraction has a cost and must be justified when it's actually needed.

## Source Code Policy *(NEW in v1.1)*

- Velnix source code is **never published, mirrored, or open-sourced**, under any license, at any stage — including Alpha, Closed Beta, Open Beta, and Release Candidate builds.
- This applies to the private GitHub repository, any CI/CD artifacts, and any third-party service Velnix integrates with.
- The public website (Netlify) may contain marketing content, documentation, download links, and license/attribution notices — never source code.
- This rule is absolute and does not require case-by-case re-justification; any process that would publish source (e.g. a misconfigured CI job, a public mirror, a "open a PR to fix this" workflow) is a defect to be fixed, not a policy exception to be granted.

## Licensing & Linking Policy *(NEW in v1.1 — replaces v1.0 "Licensing Strategy")*

- **Commercial Policy:** Velnix is closed-source, proprietary, commercial software.
- **FFmpeg:** Velnix must build against an **LGPL-configured** FFmpeg (no `--enable-gpl`, no GPL-only codecs/filters such as x264/x265 baked into the FFmpeg build itself). FFmpeg must be **dynamically linked** (separate shared library, loaded at runtime) — never statically linked into the Velnix binary. This is what allows closed-source Velnix to legally depend on FFmpeg.
  - If a GPL-only codec/filter is ever wanted (e.g. x264 encoding), it must run as an **isolated external process** communicating over a defined boundary (e.g. a subprocess or plugin), never linked into the main proprietary binary.
- **libass:** Verify current license terms before each major dependency upgrade; libass has historically been permissively licensed (ISC), but this must be re-checked, not assumed, as part of the Dependency Policy below.
- **General Dependency Policy (carried forward, sharpened):** Every dependency must be reviewed for (a) license compatibility with closed-source distribution, and (b) whether it is GPL/AGPL (which would contaminate the whole binary if statically linked) versus LGPL/MIT/BSD/Apache/ISC (which are safe under the dynamic-linking discipline above). No dependency is added without this check, regardless of how useful it is.
- Attribution and license notices for all third-party components (including dynamically linked LGPL components) must ship with the application, per LGPL's notice requirements.

## Governance Model *(NEW in v1.1)*

- Velnix is a solo project. The project owner holds final decision-making authority over all technical, design, security, and licensing decisions.
- Should the project ever take on collaborators, this section must be amended before granting anyone else decision authority — collaboration does not implicitly dilute ownership or final say.

## Architecture Principles

Modular architecture with Core, Playback, Rendering, Audio, Subtitles,
Library, UI, Security, Network, Settings, Plugins, and Diagnostics
modules.

*(See `04_Architecture/Architecture_Overview.md` for the detailed module breakdown produced under Phase 1.)*

## Performance Targets *(NEW in v1.1 — replaces v1.0 qualitative-only targets)*

Velnix commits to the following numeric targets, checked at the end of every phase, not deferred to a late "performance phase":

- **Cold start time:** < 1 second, from process launch to a responsive, playback-ready window, on reference hardware (mid-range hardware from the last ~3 years, SSD storage).
- **Idle RAM usage:** < 150 MB with no media loaded and the library index already warmed.
- **Playback CPU/GPU usage:** No specific ceiling yet defined per codec/resolution combination — to be defined during Phase 2 (Core Engine) once a baseline is measured against reference hardware, and added here as an amendment.
- **Regression rule:** Any change that regresses cold start or idle RAM beyond these targets must be fixed or made optional before merging — this is a Definition of Done gate, not a suggestion.

These targets are aggressive by design and may need renegotiation once real profiling data exists — but they are the standard to build against from day one, not retrofitted later.

## Threat Model & Security Requirements *(NEW in v1.1 — replaces v1.0's qualitative-only security section)*

Velnix treats the following as equally important attack surfaces from Phase 1 onward — none is deferred behind the others:

1. **Malicious media files** — Untrusted, potentially malformed or adversarial media containers/streams must not be able to corrupt memory, execute code, or crash the app beyond a safe, recoverable failure. Parsing/decoding paths (FFmpeg-facing) are treated as hostile input by default.
2. **Malicious subtitle files** — Subtitle formats (especially SSA/ASS via libass, which support embedded scripting/styling) are a known historical attack vector and are treated with the same suspicion as media containers. No subtitle-triggered code execution or unbounded resource use is acceptable.
3. **Malicious or misbehaving plugins** — See Plugin Security Model below. Plugins are treated as untrusted code by default, even first-party ones, until the sandbox says otherwise.
4. **Supply chain / dependency compromise** — Every dependency (including transitive ones) is reviewed before adoption; dependency updates are reviewed, not auto-merged blindly; build/release pipelines are protected against tampering (signed commits/tags where practical, no unreviewed third-party CI actions with write access to secrets).

General security requirements (carried forward from v1.0): Secure by Design, validate all inputs, sandbox media processing where practical, HTTPS-only networking, secure updates (signed packages), dependency audits, privacy-first defaults, secure plugins.

*(See `04_Architecture/Threat_Model.md` for the detailed breakdown produced under Phase 1.)*

## Plugin Security Model *(NEW in v1.1)*

Velnix plugins run under **full sandbox / process isolation**:

- Each plugin executes in its own isolated process, not in-process with the main application.
- Plugins communicate with the host application only through a defined, versioned API/IPC boundary — no direct access to host memory, arbitrary filesystem access, or arbitrary network access.
- Plugin capabilities are explicit and opt-in (e.g. "this plugin may access the network," "this plugin may read media metadata") rather than ambient/all-or-nothing.
- A plugin crashing, hanging, or misbehaving must not be able to crash or hang the main Velnix process.
- This is deliberately the higher-effort, stronger-guarantee option, chosen over a lighter permission-only model, because Phase 6 plugins are explicitly in scope for third-party code the project owner does not control.

## AI Feature Policy *(NEW in v1.1)*

- Any AI-assisted feature (e.g. scene search, subtitle translation, content suggestions) must run **locally, on-device** — no cloud inference calling out to a remote AI service.
- This is a direct consequence of the Privacy Policy's local-first, no-hidden-data-transmission commitment, and takes precedence over convenience or model quality if the two conflict.
- Each AI feature is evaluated and approved individually when it is actually proposed (expected starting Phase 6) — AI capability is not baked into the core architecture as a foundational assumption.

## UI/UX Principles

Modern, minimal, consistent, accessible, responsive, intuitive, elegant
animations.

*(See `05_UI_UX/Design_System.md` for the Phase 1 starting design system.)*

## Coding Standards

Readable, modular, documented, tested, dependency injection where
appropriate, avoid duplication and unnecessary complexity.

*(See `Coding_Standards.md` for the detailed Phase 1 standards document.)*

## Documentation Standards

Every feature must include technical documentation, user documentation
where applicable, and updated change history.

## Git Standards

Feature branches, meaningful commits, tagged releases, releasable main
branch. Repository is **private** at all times (see Source Code Policy).

## Dependency Policy

See Licensing & Linking Policy above (expanded in v1.1). Only trusted,
maintained dependencies. Remove unused packages. Verify license
compatibility before adoption, not after.

## AI Development Policy

AI-generated code must follow this constitution, preserve architecture,
avoid regressions, include error handling, and be reviewed before
merging. AI-generated code is subject to the same license-compatibility
review as any other contribution before it is merged.

## Testing Requirements

Unit, integration, UI, performance, compatibility, regression, and
security testing.

## Definition of Done

Code works, tests pass, documentation updated, security reviewed
against the Threat Model, performance reviewed against the numeric
Performance Targets above, no critical defects.

## Privacy Policy

No hidden telemetry. No unnecessary data collection. Local processing
whenever practical. User control over privacy. AI features specifically
must remain local-only per the AI Feature Policy above.

## Release Strategy *(clarified in v1.1)*

Internal → Alpha → Closed Beta → Open Beta → Release Candidate → Stable.

There is no dedicated "release phase" separate from development. Alpha,
Closed Beta, and Open Beta testing occur continuously across Phases 4–8
of the development roadmap (Playback Features through Testing & QA) as
features become testable, rather than being batched into Phase 9. Phase
9 (Release) covers Release Candidate hardening, installer/portable
build packaging, and Stable release itself.

## Long-Term Support / Maintenance Policy *(NEW in v1.1)*

- A v1.x maintenance branch is established as soon as v1.0 ships, to receive bug fixes and security patches independent of v2.x (Plugins) development.
- v2.x feature work (plugins, advanced customization) proceeds on its own branch/track and does not block v1.x patch releases.
- This policy exists now, before it's needed, specifically so that plugin/cloud/AI expansion in v2.x/v3.x never comes at the cost of leaving v1.x users without security patches.

## Long-Term Roadmap

v1.x: Core player (Windows-only). v2.x: Plugins and advanced
customization (cross-platform evaluation may begin here). v3.x:
Optional AI and cloud capabilities (subject to the local-only AI
Feature Policy above, re-evaluated if true cloud sync features are
ever proposed — such a proposal would itself require a constitutional
amendment given the current local-first privacy commitment).

## Constitutional Rule

No change may violate this constitution unless the constitution itself
is formally amended and versioned.

# GOLDEN RULE

Quality is the highest priority in the Velnix project.

Velnix will never ship a feature simply because it is quick or easy to
implement. Every feature, enhancement, optimization, bug fix, and UI
change must satisfy the project's engineering standards before it is
considered complete.

Every contribution must meet: Performance, Security, Reliability,
Maintainability, User Experience, Compatibility, Testing, Documentation.

If a conflict exists between development speed and software quality,
quality always takes precedence.

# PERFORMANCE WITHOUT COMPROMISE

Every new feature must justify its resource usage against the numeric
Performance Targets above. If a feature negatively affects startup
time, playback smoothness, memory consumption, battery life, or
responsiveness, it must be redesigned, optimized, or made optional
before release.

---

## Amendment Log

- **v1.0 → v1.1 (2026-08-03):** Added Platform Scope, Source Code Policy, Licensing & Linking Policy (FFmpeg LGPL/dynamic-linking discipline), Governance Model, numeric Performance Targets, Threat Model & Security Requirements, Plugin Security Model (full sandbox/process isolation), AI Feature Policy (local-only), clarified Release Strategy mapping onto Phases 4–8, and Long-Term Support/Maintenance Policy. No prior sections were removed; all v1.0 content not superseded above carries forward unchanged.
