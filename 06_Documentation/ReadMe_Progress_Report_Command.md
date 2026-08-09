# COMMAND: Okay-Great-Pack

Whenever I type **"Okay-Great-Pack"**, execute the following workflow automatically.

## Objective

Create or update a Markdown handoff package that records everything completed in the current development step of **Velnix**.

This document becomes the official project memory for the next development session.

Every future session MUST read this document before making any changes.

This command is intended for use **only while Velnix is being actively built** — it exists so you (or any future session) can quickly find out where the project stopped and where to pick back up.

---

# Instructions

1. Analyze the entire conversation and every modification completed during this step.

2. Create (or update) a Markdown file named:

Step_##_Progress_Report.md

Example:

Step_01_Progress_Report.md
Step_02_Progress_Report.md
Step_03_Progress_Report.md
...

Store it inside:

/06_Documentation/Progress_Reports/

Create the folder if it does not exist.

---

# The report MUST contain

# Step Title

Example

Phase 2 — Core Playback Engine (Codec Integration)

---

# Date

Current date and time.

---

# Phase Reference

Which of the 9 Velnix phases this step belongs to (e.g. "Phase 2 — Core Engine").

---

# Objective

Explain what this step was intended to accomplish.

---

# Completed Work

List every completed task.

Example

- Integrated FFmpeg decoding pipeline
- Implemented hardware-accelerated video rendering path
- Fixed audio/video sync drift on VFR files
- Added subtitle rendering via libass
- Improved cold-start time to under 1s

---

# Files Modified

List every file changed.

---

# Files Created

List every newly created file.

---

# Files Deleted

List every removed file.

---

# Architecture Changes

Document any changes made to:

- module structure (Core, Playback, Rendering, Audio, Subtitles, Library, UI, Security, Network, Settings, Plugins, Diagnostics)
- FFmpeg/libass integration or linking approach (must remain LGPL-compliant dynamic linking)
- plugin sandbox/isolation boundary
- data storage (local settings, cache, library index)
- update service

---

# Dependencies Added

Include

- Package Name
- Version
- Reason
- License (must be commercially compatible — no GPL contamination of closed-source code)

Example

- FFmpeg | 6.1 (LGPL build, dynamically linked) | Media decoding | LGPL 2.1

---

# Issues Fixed

Describe every bug resolved.

---

# Known Issues

List anything still unresolved.

---

# Technical Debt

List shortcuts that should be improved later.

---

# Security Review

Document status against Velnix's threat model:

- malicious media file handling (parser/codec hardening)
- malicious subtitle file handling (libass/SSA script safety)
- plugin sandboxing / process isolation
- dependency/supply-chain review
- secure update delivery (HTTPS, signed packages)

---

# Performance Review

Document status against Velnix's numeric targets:

- Cold start time (target: <1s)
- Idle RAM usage (target: <150MB)
- CPU/GPU usage during playback
- Any regressions introduced this step

---

# Testing Completed

List

- manual tests
- unit tests
- integration tests
- performance tests
- compatibility tests (codecs/containers/hardware)

---

# Deployment / Build Status

Document

- Development build
- Internal testing
- Alpha / Beta / RC status (per constitution's release stages)
- Installer build status
- Portable build status
- GitHub repository status (private — source is never published)
- Netlify (website only — not application source)

---

# Next Recommended Tasks

Generate an ordered checklist.

Example

- Finish subtitle style override UI
- Optimize library scan performance
- Begin plugin sandbox API design
- Add crash reporting opt-in flow

---

# Developer Notes

Include important observations for future development.

Explain WHY decisions were made.

Document anything a future session should know before touching this area again.

---

# AI Handoff Summary

Write a concise summary that another AI can immediately understand.

Include

- Current project state
- Completed features
- Pending work
- Known limitations
- Warnings (e.g. licensing constraints, security-sensitive areas)
- Required next actions

This section should allow another AI to continue the project without needing the previous conversation.

---

# Change Log

Maintain a running log.

Example

Version 0.2

- Added FFmpeg playback pipeline
- Improved cold-start performance
- Fixed subtitle sync bug

---

# Completion Status

Overall completion percentage, tracked against the 9-phase plan.

Example

- Phase 1 — Foundation & Planning: 100%
- Phase 2 — Core Engine: 60%
- Phase 3 — Modern UI & UX: 0%
- Phase 4 — Playback Features: 0%
- Phase 5 — Media Library: 0%
- Phase 6 — Advanced Features: 0%
- Phase 7 — Performance & Security: 20%
- Phase 8 — Testing & QA: 10%
- Phase 9 — Release: 0%
- Overall Project: 22%

---

# Deliverables

At the end of the command

1. Save the report.
2. Verify all sections are completed.
3. Ensure no completed work is omitted.
4. Keep previous reports untouched.
5. Create only one new report for the current step.
6. Never overwrite previous reports.
7. Maintain consistent formatting across all reports.
8. The report must serve as the single source of truth for the next development session.
9. Never include or reference application source code in this report — progress reports are documentation only, not a code archive.
