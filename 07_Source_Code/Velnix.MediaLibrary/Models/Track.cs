namespace Velnix.MediaLibrary.Models;

/// <summary>
/// A single indexed media file. Metadata fields are local-only for v1.x
/// (no online poster/description fetching) and are nullable because tag
/// extraction can fail or be partial for a given file.
/// </summary>
public sealed class Track
{
    public long Id { get; init; }
    public required string FilePath { get; init; }
    public long FileSizeBytes { get; init; }
    public DateTime LastModifiedUtc { get; init; }
    public long SourceRootId { get; init; }

    public string? Title { get; init; }
    public string? Artist { get; init; }
    public string? Album { get; init; }
    public string? Genre { get; init; }
    public int? Year { get; init; }
    public int? TrackNumber { get; init; }
    public int? DurationMs { get; init; }
    public string? Container { get; init; }
    public string? VideoCodec { get; init; }
    public string? AudioCodec { get; init; }
    public int? Width { get; init; }
    public int? Height { get; init; }

    /// <summary>Raw JSON bucket for metadata fields not yet promoted to a column.</summary>
    public string? MetadataExtraJson { get; init; }

    public DateTime AddedUtc { get; init; }

    /// <summary>True if the file was not found on the most recent scan.</summary>
    public bool Missing { get; init; }
}
