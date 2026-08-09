using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

// Owned here per the Phase 4/5 coordination note: Phase 4's bookmarks and
// chapter caching use this phase's SQLite schema rather than a separate store.

public sealed class BookmarkRepository
{
    private readonly LibraryDatabase _db;

    public BookmarkRepository(LibraryDatabase db) => _db = db;

    public long Add(long trackId, int positionMs, string? label)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            INSERT INTO bookmarks (track_id, position_ms, label, created_utc)
            VALUES (@tid, @pos, @label, @now)
            RETURNING id;
            """;
        cmd.Parameters.AddWithValue("@tid", trackId);
        cmd.Parameters.AddWithValue("@pos", positionMs);
        cmd.Parameters.AddWithValue("@label", (object?)label ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@now", DateTime.UtcNow.ToString("o"));
        return (long)cmd.ExecuteScalar()!;
    }

    public void Remove(long bookmarkId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM bookmarks WHERE id = @id;";
        cmd.Parameters.AddWithValue("@id", bookmarkId);
        cmd.ExecuteNonQuery();
    }

    public IReadOnlyList<Bookmark> GetForTrack(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT * FROM bookmarks WHERE track_id = @tid ORDER BY position_ms;";
        cmd.Parameters.AddWithValue("@tid", trackId);
        using var reader = cmd.ExecuteReader();

        var results = new List<Bookmark>();
        while (reader.Read())
        {
            results.Add(new Bookmark
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                TrackId = reader.GetInt64(reader.GetOrdinal("track_id")),
                PositionMs = reader.GetInt32(reader.GetOrdinal("position_ms")),
                Label = reader.IsDBNull(reader.GetOrdinal("label")) ? null : reader.GetString(reader.GetOrdinal("label")),
                CreatedUtc = DateTime.Parse(reader.GetString(reader.GetOrdinal("created_utc"))),
            });
        }
        return results;
    }
}

public sealed class ChapterCacheRepository
{
    private readonly LibraryDatabase _db;

    public ChapterCacheRepository(LibraryDatabase db) => _db = db;

    public void ReplaceForTrack(long trackId, IReadOnlyList<(int StartMs, string? Title)> chapters)
    {
        using var conn = _db.OpenConnection();
        using var tx = conn.BeginTransaction();

        using (var del = conn.CreateCommand())
        {
            del.Transaction = tx;
            del.CommandText = "DELETE FROM chapters_cache WHERE track_id = @tid;";
            del.Parameters.AddWithValue("@tid", trackId);
            del.ExecuteNonQuery();
        }

        foreach (var ch in chapters)
        {
            using var insert = conn.CreateCommand();
            insert.Transaction = tx;
            insert.CommandText = """
                INSERT INTO chapters_cache (track_id, start_ms, title)
                VALUES (@tid, @start, @title);
                """;
            insert.Parameters.AddWithValue("@tid", trackId);
            insert.Parameters.AddWithValue("@start", ch.StartMs);
            insert.Parameters.AddWithValue("@title", (object?)ch.Title ?? DBNull.Value);
            insert.ExecuteNonQuery();
        }

        tx.Commit();
    }

    public IReadOnlyList<ChapterCacheEntry> GetForTrack(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT * FROM chapters_cache WHERE track_id = @tid ORDER BY start_ms;";
        cmd.Parameters.AddWithValue("@tid", trackId);
        using var reader = cmd.ExecuteReader();

        var results = new List<ChapterCacheEntry>();
        while (reader.Read())
        {
            results.Add(new ChapterCacheEntry
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                TrackId = reader.GetInt64(reader.GetOrdinal("track_id")),
                StartMs = reader.GetInt32(reader.GetOrdinal("start_ms")),
                Title = reader.IsDBNull(reader.GetOrdinal("title")) ? null : reader.GetString(reader.GetOrdinal("title")),
            });
        }
        return results;
    }
}
