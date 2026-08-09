using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

public sealed class FavoritesRepository
{
    private readonly LibraryDatabase _db;

    public FavoritesRepository(LibraryDatabase db) => _db = db;

    public void Add(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            INSERT INTO favorites (track_id, added_utc) VALUES (@id, @now)
            ON CONFLICT(track_id) DO NOTHING;
            """;
        cmd.Parameters.AddWithValue("@id", trackId);
        cmd.Parameters.AddWithValue("@now", DateTime.UtcNow.ToString("o"));
        cmd.ExecuteNonQuery();
    }

    public void Remove(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM favorites WHERE track_id = @id;";
        cmd.Parameters.AddWithValue("@id", trackId);
        cmd.ExecuteNonQuery();
    }

    public bool IsFavorite(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT COUNT(*) FROM favorites WHERE track_id = @id;";
        cmd.Parameters.AddWithValue("@id", trackId);
        return Convert.ToInt64(cmd.ExecuteScalar()) > 0;
    }

    public IReadOnlyList<long> GetAllTrackIds()
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT track_id FROM favorites ORDER BY added_utc DESC;";
        using var reader = cmd.ExecuteReader();

        var results = new List<long>();
        while (reader.Read()) results.Add(reader.GetInt64(0));
        return results;
    }
}
