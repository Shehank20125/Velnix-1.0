using Microsoft.Data.Sqlite;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

public sealed class LibraryRootRepository
{
    private readonly LibraryDatabase _db;

    public LibraryRootRepository(LibraryDatabase db) => _db = db;

    public long Add(string path, bool isNetworkPath, int rescanIntervalMinutes = 15)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            INSERT INTO library_roots (path, is_network_path, enabled, rescan_interval_minutes)
            VALUES (@path, @isNet, 1, @interval)
            ON CONFLICT(path) DO UPDATE SET is_network_path = excluded.is_network_path
            RETURNING id;
            """;
        cmd.Parameters.AddWithValue("@path", path);
        cmd.Parameters.AddWithValue("@isNet", isNetworkPath ? 1 : 0);
        cmd.Parameters.AddWithValue("@interval", rescanIntervalMinutes);
        return (long)cmd.ExecuteScalar()!;
    }

    public void SetLastScanned(long rootId, DateTime utc)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "UPDATE library_roots SET last_scanned_utc = @t WHERE id = @id;";
        cmd.Parameters.AddWithValue("@t", utc.ToString("o"));
        cmd.Parameters.AddWithValue("@id", rootId);
        cmd.ExecuteNonQuery();
    }

    public IReadOnlyList<LibraryRoot> GetAll()
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT * FROM library_roots ORDER BY path;";
        using var reader = cmd.ExecuteReader();

        var results = new List<LibraryRoot>();
        while (reader.Read())
        {
            results.Add(new LibraryRoot
            {
                Id = reader.GetInt64(reader.GetOrdinal("id")),
                Path = reader.GetString(reader.GetOrdinal("path")),
                IsNetworkPath = reader.GetInt32(reader.GetOrdinal("is_network_path")) == 1,
                Enabled = reader.GetInt32(reader.GetOrdinal("enabled")) == 1,
                LastScannedUtc = reader.IsDBNull(reader.GetOrdinal("last_scanned_utc"))
                    ? null
                    : DateTime.Parse(reader.GetString(reader.GetOrdinal("last_scanned_utc"))),
                RescanIntervalMinutes = reader.GetInt32(reader.GetOrdinal("rescan_interval_minutes")),
            });
        }
        return results;
    }
}
