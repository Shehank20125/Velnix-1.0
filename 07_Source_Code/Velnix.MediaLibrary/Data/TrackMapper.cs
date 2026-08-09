using Microsoft.Data.Sqlite;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Data;

internal static class TrackMapper
{
    public static Track Map(SqliteDataReader r) => new()
    {
        Id = r.GetInt64(r.GetOrdinal("id")),
        FilePath = r.GetString(r.GetOrdinal("file_path")),
        FileSizeBytes = r.GetInt64(r.GetOrdinal("file_size_bytes")),
        LastModifiedUtc = DateTime.Parse(r.GetString(r.GetOrdinal("last_modified_utc"))),
        SourceRootId = r.GetInt64(r.GetOrdinal("source_root_id")),
        Title = r.IsDBNull(r.GetOrdinal("title")) ? null : r.GetString(r.GetOrdinal("title")),
        Artist = r.IsDBNull(r.GetOrdinal("artist")) ? null : r.GetString(r.GetOrdinal("artist")),
        Album = r.IsDBNull(r.GetOrdinal("album")) ? null : r.GetString(r.GetOrdinal("album")),
        Genre = r.IsDBNull(r.GetOrdinal("genre")) ? null : r.GetString(r.GetOrdinal("genre")),
        Year = r.IsDBNull(r.GetOrdinal("year")) ? null : r.GetInt32(r.GetOrdinal("year")),
        TrackNumber = r.IsDBNull(r.GetOrdinal("track_number")) ? null : r.GetInt32(r.GetOrdinal("track_number")),
        DurationMs = r.IsDBNull(r.GetOrdinal("duration_ms")) ? null : r.GetInt32(r.GetOrdinal("duration_ms")),
        Container = r.IsDBNull(r.GetOrdinal("container")) ? null : r.GetString(r.GetOrdinal("container")),
        VideoCodec = r.IsDBNull(r.GetOrdinal("video_codec")) ? null : r.GetString(r.GetOrdinal("video_codec")),
        AudioCodec = r.IsDBNull(r.GetOrdinal("audio_codec")) ? null : r.GetString(r.GetOrdinal("audio_codec")),
        Width = r.IsDBNull(r.GetOrdinal("width")) ? null : r.GetInt32(r.GetOrdinal("width")),
        Height = r.IsDBNull(r.GetOrdinal("height")) ? null : r.GetInt32(r.GetOrdinal("height")),
        MetadataExtraJson = r.IsDBNull(r.GetOrdinal("metadata_extra_json")) ? null : r.GetString(r.GetOrdinal("metadata_extra_json")),
        AddedUtc = DateTime.Parse(r.GetString(r.GetOrdinal("added_utc"))),
        Missing = r.GetInt32(r.GetOrdinal("missing")) == 1,
    };
}
