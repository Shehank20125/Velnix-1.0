using Microsoft.Data.Sqlite;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data.Repositories;

public sealed class TrackRepository
{
    private readonly LibraryDatabase _db;

    public TrackRepository(LibraryDatabase db) => _db = db;

    /// <summary>Used by the scanner to report accurate added-vs-updated counts.</summary>
    public bool ExistsByPath(string filePath)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT COUNT(*) FROM tracks WHERE file_path = @path;";
        cmd.Parameters.AddWithValue("@path", filePath);
        return Convert.ToInt64(cmd.ExecuteScalar()) > 0;
    }

    public long Upsert(Track track)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            INSERT INTO tracks
                (file_path, file_size_bytes, last_modified_utc, source_root_id,
                 title, artist, album, genre, year, track_number, duration_ms,
                 container, video_codec, audio_codec, width, height,
                 metadata_extra_json, added_utc, missing)
            VALUES
                (@path, @size, @modified, @rootId,
                 @title, @artist, @album, @genre, @year, @trackNo, @duration,
                 @container, @videoCodec, @audioCodec, @width, @height,
                 @extra, @added, 0)
            ON CONFLICT(file_path) DO UPDATE SET
                file_size_bytes = excluded.file_size_bytes,
                last_modified_utc = excluded.last_modified_utc,
                title = excluded.title,
                artist = excluded.artist,
                album = excluded.album,
                genre = excluded.genre,
                year = excluded.year,
                track_number = excluded.track_number,
                duration_ms = excluded.duration_ms,
                container = excluded.container,
                video_codec = excluded.video_codec,
                audio_codec = excluded.audio_codec,
                width = excluded.width,
                height = excluded.height,
                metadata_extra_json = excluded.metadata_extra_json,
                missing = 0
            RETURNING id;
            """;

        cmd.Parameters.AddWithValue("@path", track.FilePath);
        cmd.Parameters.AddWithValue("@size", track.FileSizeBytes);
        cmd.Parameters.AddWithValue("@modified", track.LastModifiedUtc.ToString("o"));
        cmd.Parameters.AddWithValue("@rootId", track.SourceRootId);
        cmd.Parameters.AddWithValue("@title", (object?)track.Title ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@artist", (object?)track.Artist ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@album", (object?)track.Album ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@genre", (object?)track.Genre ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@year", (object?)track.Year ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@trackNo", (object?)track.TrackNumber ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@duration", (object?)track.DurationMs ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@container", (object?)track.Container ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@videoCodec", (object?)track.VideoCodec ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@audioCodec", (object?)track.AudioCodec ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@width", (object?)track.Width ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@height", (object?)track.Height ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@extra", (object?)track.MetadataExtraJson ?? DBNull.Value);
        cmd.Parameters.AddWithValue("@added", DateTime.UtcNow.ToString("o"));

        return (long)cmd.ExecuteScalar()!;
    }

    public void MarkMissingExcept(long sourceRootId, IReadOnlyCollection<string> stillPresentPaths)
    {
        using var conn = _db.OpenConnection();
        using var tx = conn.BeginTransaction();

        using var cmd = conn.CreateCommand();
        cmd.Transaction = tx;
        cmd.CommandText = """
            UPDATE tracks SET missing = 1
            WHERE source_root_id = @rootId
              AND file_path NOT IN (
                  SELECT value FROM json_each(@present)
              );
            """;
        cmd.Parameters.AddWithValue("@rootId", sourceRootId);
        cmd.Parameters.AddWithValue("@present",
            System.Text.Json.JsonSerializer.Serialize(stillPresentPaths));
        cmd.ExecuteNonQuery();

        tx.Commit();
    }

    public void Delete(long trackId)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM tracks WHERE id = @id;";
        cmd.Parameters.AddWithValue("@id", trackId);
        cmd.ExecuteNonQuery();
    }

    public IReadOnlyList<Track> GetAll(bool includeMissing = false)
    {
        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = includeMissing
            ? "SELECT * FROM tracks ORDER BY artist, album, track_number, title;"
            : "SELECT * FROM tracks WHERE missing = 0 ORDER BY artist, album, track_number, title;";

        using var reader = cmd.ExecuteReader();
        var results = new List<Track>();
        while (reader.Read())
        {
            results.Add(Map(reader));
        }
        return results;
    }

    private static Track Map(SqliteDataReader r) => TrackMapper.Map(r);
}
