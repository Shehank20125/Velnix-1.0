-- Velnix Media Library — SQLite schema (v1)
-- Engine: Microsoft.Data.Sqlite (MIT). Local-only metadata for v1.x.
-- Schema is intentionally denormalized-light and extensible: new metadata
-- columns/tables can be added later without breaking v1 readers, since we
-- avoid SELECT * in repositories and always project explicit columns.

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- Schema version tracking for future migrations.
CREATE TABLE IF NOT EXISTS schema_version (
    version     INTEGER NOT NULL,
    applied_utc TEXT    NOT NULL
);

-- One row per discovered media file. This is the library index.
CREATE TABLE IF NOT EXISTS tracks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    file_path       TEXT    NOT NULL UNIQUE,
    file_size_bytes INTEGER NOT NULL,
    last_modified_utc TEXT NOT NULL,
    source_root_id  INTEGER NOT NULL REFERENCES library_roots(id) ON DELETE CASCADE,

    -- Local-only metadata (no online fetch in v1.x). All nullable: metadata
    -- may not be resolvable for every file (e.g. corrupt tags).
    title           TEXT,
    artist          TEXT,
    album           TEXT,
    genre           TEXT,
    year            INTEGER,
    track_number    INTEGER,
    duration_ms     INTEGER,
    container       TEXT,
    video_codec     TEXT,
    audio_codec     TEXT,
    width           INTEGER,
    height          INTEGER,

    -- Extensible metadata bucket for anything not worth a dedicated column
    -- yet. Keeps schema additive rather than requiring migrations for every
    -- new tag we decide to surface later (e.g. online poster art in a
    -- future phase).
    metadata_extra_json TEXT,

    added_utc       TEXT    NOT NULL,
    missing         INTEGER NOT NULL DEFAULT 0 -- 1 = file no longer found on rescan, kept for history/favorites integrity
);

CREATE INDEX IF NOT EXISTS idx_tracks_title  ON tracks(title);
CREATE INDEX IF NOT EXISTS idx_tracks_artist ON tracks(artist);
CREATE INDEX IF NOT EXISTS idx_tracks_album  ON tracks(album);
CREATE INDEX IF NOT EXISTS idx_tracks_missing ON tracks(missing);

-- Full-text search over the searchable fields. Kept in sync via triggers.
CREATE VIRTUAL TABLE IF NOT EXISTS tracks_fts USING fts5(
    title, artist, album, genre,
    content='tracks',
    content_rowid='id'
);

CREATE TRIGGER IF NOT EXISTS tracks_ai AFTER INSERT ON tracks BEGIN
    INSERT INTO tracks_fts(rowid, title, artist, album, genre)
    VALUES (new.id, new.title, new.artist, new.album, new.genre);
END;

CREATE TRIGGER IF NOT EXISTS tracks_ad AFTER DELETE ON tracks BEGIN
    INSERT INTO tracks_fts(tracks_fts, rowid, title, artist, album, genre)
    VALUES ('delete', old.id, old.title, old.artist, old.album, old.genre);
END;

CREATE TRIGGER IF NOT EXISTS tracks_au AFTER UPDATE ON tracks BEGIN
    INSERT INTO tracks_fts(tracks_fts, rowid, title, artist, album, genre)
    VALUES ('delete', old.id, old.title, old.artist, old.album, old.genre);
    INSERT INTO tracks_fts(rowid, title, artist, album, genre)
    VALUES (new.id, new.title, new.artist, new.album, new.genre);
END;

-- Configured library folders. FileSystemWatcher is attached per local root;
-- network roots (UNC paths) rely on the scheduled rescan fallback instead,
-- since FileSystemWatcher is known to be unreliable over SMB.
CREATE TABLE IF NOT EXISTS library_roots (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    path            TEXT    NOT NULL UNIQUE,
    is_network_path INTEGER NOT NULL DEFAULT 0,
    enabled         INTEGER NOT NULL DEFAULT 1,
    last_scanned_utc TEXT,
    rescan_interval_minutes INTEGER NOT NULL DEFAULT 15 -- only used when is_network_path = 1
);

-- Playlists (user-created, ordered).
CREATE TABLE IF NOT EXISTS playlists (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL,
    created_utc TEXT    NOT NULL,
    updated_utc TEXT    NOT NULL
);

CREATE TABLE IF NOT EXISTS playlist_items (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    playlist_id INTEGER NOT NULL REFERENCES playlists(id) ON DELETE CASCADE,
    track_id    INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE,
    sort_order  INTEGER NOT NULL,
    UNIQUE(playlist_id, sort_order)
);

CREATE INDEX IF NOT EXISTS idx_playlist_items_playlist ON playlist_items(playlist_id);

-- Favorites: simple many-to-none flag table, kept separate from tracks so
-- favoriting a missing/deleted file doesn't require touching the tracks row.
CREATE TABLE IF NOT EXISTS favorites (
    track_id    INTEGER PRIMARY KEY REFERENCES tracks(id) ON DELETE CASCADE,
    added_utc   TEXT    NOT NULL
);

-- Playback history. Append-only log; one row per playback session start.
CREATE TABLE IF NOT EXISTS history (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    track_id        INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE,
    played_utc      TEXT    NOT NULL,
    last_position_ms INTEGER NOT NULL DEFAULT 0,
    completed       INTEGER NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_history_track ON history(track_id);
CREATE INDEX IF NOT EXISTS idx_history_played ON history(played_utc);

-- Phase 4 coordination: bookmarks and chapter caching now live here rather
-- than in a separate store, per the locked planning decision.
CREATE TABLE IF NOT EXISTS bookmarks (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    track_id    INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE,
    position_ms INTEGER NOT NULL,
    label       TEXT,
    created_utc TEXT    NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_bookmarks_track ON bookmarks(track_id);

CREATE TABLE IF NOT EXISTS chapters_cache (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    track_id    INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE,
    start_ms    INTEGER NOT NULL,
    title       TEXT,
    UNIQUE(track_id, start_ms)
);

CREATE INDEX IF NOT EXISTS idx_chapters_track ON chapters_cache(track_id);
