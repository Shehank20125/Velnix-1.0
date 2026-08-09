# Velnix.MediaLibrary — Phase 5 Implementation (Initial Scaffold)

Status: **Implementation started.** Core library-index, search, playlists,
favorites, history, and bookmark/chapter storage are implemented. This
module is self-contained and has not yet been wired into the rest of the
Velnix solution — see "Integration steps required" below.

## What's implemented

- **Storage engine**: SQLite via `Microsoft.Data.Sqlite` (MIT), per the
  locked planning decision. Dynamic linking only — no static linking of
  GPL components, no GPL dependencies added.
- **Schema** (`Schema/001_initial_schema.sql`): tracks, library_roots,
  playlists/playlist_items, favorites, history, bookmarks, chapters_cache,
  plus an FTS5 virtual table for search, kept in sync via triggers.
- **Library scanning** (`Services/LibraryScanService.cs`,
  `Services/LibraryWatcherService.cs`): real-time `FileSystemWatcher` for
  local roots with debounced rescans; timer-based scheduled rescan for
  network (UNC) roots, since `FileSystemWatcher` is unreliable over SMB.
  Missing files are marked `missing = 1` rather than deleted, so
  favorites/history/playlists referencing them survive a temporarily
  offline drive or share.
- **Metadata**: local-only for v1.x, matching the plan. Schema has an
  extensible `metadata_extra_json` bucket so new fields don't require a
  migration for every tag we later decide to surface.
- **Search**: FTS5 prefix search over title/artist/album/genre.
- **Playlists, favorites, history, bookmarks, chapter cache**: full CRUD
  repositories.

## Integration steps required (next session)

1. **Wire `IMediaMetadataProbe`.** This module intentionally does not
   implement tag/codec probing — Phase 2's FFmpeg-based decoding pipeline
   already owns that. Implement `IMediaMetadataProbe` as a thin adapter
   over the existing avformat/avcodec metadata extraction and inject it
   into `LibraryScanService`. Until this is wired, tracks are indexed with
   filesystem info only (path, size, modified time, extension).
2. **Decide the on-disk database path.** `LibraryDatabase` takes an
   explicit file path rather than hardcoding one, so it defers to
   whatever module owns local settings/cache storage. Confirm that path
   with whichever module that is and pass it in at startup.
3. **Call `LibraryDatabase.EnsureSchema()` once at app startup**, then
   construct `LibraryWatcherService` and call `Start()` after library
   roots are loaded from `LibraryRootRepository`.
4. **UI hookup**: none of this has a UI yet (Phase 3 — Modern UI & UX is
   still 0%). Repositories/services here are UI-agnostic and can be called
   from whatever view-model layer Phase 3 introduces.
5. **Add unit tests** against `LibraryScanService` using a fake
   `IMediaMetadataProbe` (the interface was designed for this).

## Known gaps / not yet done

- No thumbnail/poster art (out of scope for v1.x — local-only metadata).
- No duplicate-file detection.
- No large-library performance testing yet (target: acceptable scan time
  on libraries in the tens of thousands of files — untested). Note that
  `LibraryScanService` currently does one extra existence-check query per
  file (to report accurate added-vs-updated counts); worth revisiting if
  scan time on very large libraries becomes an issue.

## Licensing

- `Microsoft.Data.Sqlite` — MIT. Commercially safe, no GPL contamination.
- No other new dependencies introduced.
