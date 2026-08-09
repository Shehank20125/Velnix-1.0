using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

public sealed class HistoryRepository
{
    private readonly LibraryDatabase _db;

    public HistoryRepository(LibraryDatabase db) => _db = db;

    public long RecordPlaybackStart(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            INSERT INTO history (track_id, played_utc, last_position_ms, completed)
            VALUES (@id, @now, 0, 0)
            RETURNING id;
            """;
        cmd.Parameters.AddWithValue("@id", trackId);
        cmd.Parameters.AddWithValue("@now", DateTime.UtcNow.ToString("o"));
        return (long)cmd.ExecuteScalar()!;
    }

    public void UpdateProgress(long historyId, int lastPositionMs, bool completed)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            UPDATE history SET last_position_ms = @pos, completed = @done
            WHERE id = @id;
            """;
        cmd.Parameters.AddWithValue("@pos", lastPositionMs);
        cmd.Parameters.AddWithValue("@done", completed ? 1 : 0);
        cmd.Parameters.AddWithValue("@id", historyId);
        cmd.ExecuteNonQuery();
    }

    public IReadOnlyList<HistoryEntry> GetRecent(int limit = 50)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT * FROM history ORDER BY played_utc DESC LIMIT @limit;";
        cmd.Parameters.AddWithValue("@limit", limit);
        using var reader = cmd.ExecuteReader();

        var results = new List<HistoryEntry>();
        while (reader.Read())
        {
            results.Add(new HistoryEntry
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                TrackId = reader.GetInt64(reader.GetOrdinal("track_id")),
                PlayedUtc = DateTime.Parse(reader.GetString(reader.GetOrdinal("played_utc"))),
                LastPositionMs = reader.GetInt32(reader.GetOrdinal("last_position_ms")),
                Completed = reader.GetInt32(reader.GetOrdinal("completed")) == 1,
            });
        }
        return results;
    }
}
