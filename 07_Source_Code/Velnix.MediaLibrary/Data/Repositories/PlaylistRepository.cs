using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

public sealed class PlaylistRepository
{
    private readonly LibraryDatabase _db;

    public PlaylistRepository(LibraryDatabase db) => _db = db;

    public long Create(string name)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        var now = DateTime.UtcNow.ToString("o");
        cmd.CommandText = """
            INSERT INTO playlists (name, created_utc, updated_utc)
            VALUES (@name, @now, @now)
            RETURNING id;
            """;
        cmd.Parameters.AddWithValue("@name", name);
        cmd.Parameters.AddWithValue("@now", now);
        return (long)cmd.ExecuteScalar()!;
    }

    public void Rename(long playlistId, string newName)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "UPDATE playlists SET name = @name, updated_utc = @now WHERE id = @id;";
        cmd.Parameters.AddWithValue("@name", newName);
        cmd.Parameters.AddWithValue("@now", DateTime.UtcNow.ToString("o"));
        cmd.Parameters.AddWithValue("@id", playlistId);
        cmd.ExecuteNonQuery();
    }

    public void Delete(long playlistId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM playlists WHERE id = @id;";
        cmd.Parameters.AddWithValue("@id", playlistId);
        cmd.ExecuteNonQuery();
    }

    public void AddTrack(long playlistId, long trackId)
    {
        using var conn = _db.OpenConnection();
        using var tx = conn.BeginTransaction();

        int nextOrder;
        using (var maxCmd = conn.CreateCommand())
        {
            maxCmd.Transaction = tx;
            maxCmd.CommandText =
                "SELECT COALESCE(MAX(sort_order), -1) + 1 FROM playlist_items WHERE playlist_id = @pid;";
            maxCmd.Parameters.AddWithValue("@pid", playlistId);
            nextOrder = Convert.ToInt32(maxCmd.ExecuteScalar());
        }

        using (var insert = conn.CreateCommand())
        {
            insert.Transaction = tx;
            insert.CommandText = """
                INSERT INTO playlist_items (playlist_id, track_id, sort_order)
                VALUES (@pid, @tid, @order);
                """;
            insert.Parameters.AddWithValue("@pid", playlistId);
            insert.Parameters.AddWithValue("@tid", trackId);
            insert.Parameters.AddWithValue("@order", nextOrder);
            insert.ExecuteNonQuery();
        }

        tx.Commit();
    }

    public void RemoveItem(long playlistItemId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM playlist_items WHERE id = @id;";
        cmd.Parameters.AddWithValue("@id", playlistItemId);
        cmd.ExecuteNonQuery();
    }

    public IReadOnlyList<Playlist> GetAll()
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT * FROM playlists ORDER BY name;";
        using var reader = cmd.ExecuteReader();

        var results = new List<Playlist>();
        while (reader.Read())
        {
            results.Add(new Playlist
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                Name = reader.GetString(reader.GetOrdinal("name")),
                CreatedUtc = DateTime.Parse(reader.GetString(reader.GetOrdinal("created_utc"))),
                UpdatedUtc = DateTime.Parse(reader.GetString(reader.GetOrdinal("updated_utc"))),
            });
        }
        return results;
    }

    public IReadOnlyList<PlaylistItem> GetItems(long playlistId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText =
            "SELECT * FROM playlist_items WHERE playlist_id = @pid ORDER BY sort_order;";
        cmd.Parameters.AddWithValue("@pid", playlistId);
        using var reader = cmd.ExecuteReader();

        var results = new List<PlaylistItem>();
        while (reader.Read())
        {
            results.Add(new PlaylistItem
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                PlaylistId = reader.GetInt64(reader.GetOrdinal("playlist_id")),
                TrackId = reader.GetInt64(reader.GetOrdinal("track_id")),
                SortOrder = reader.GetInt32(reader.GetOrdinal("sort_order")),
            });
        }
        return results;
    }
}
