namespace Velnix.MediaLibrary.Services;

/// <summary>
/// Extracts local media tags/technical info from a file on disk.
///
/// INTEGRATION POINT: this module deliberately does not implement tag
/// reading itself. Velnix's Core/Playback modules already own the
/// FFmpeg-based decoding pipeline (Phase 2), so the concrete
/// implementation of this interface should wrap that existing probing
/// code (e.g. avformat/avcodec metadata dictionaries) rather than
/// pulling in a second, separate tagging library. Keeping this as an
/// interface lets the library-scan service be built and unit-tested
/// against a fake probe before that wiring exists.
/// </summary>
public interface IMediaMetadataProbe
{
    /// <summary>Return null if the file cannot be probed (corrupt/unsupported) — the
    /// scanner still indexes the file with filesystem-only info in that case.</summary>
    ProbedMetadata? Probe(string filePath);
}

public sealed record ProbedMetadata(
    string? Title,
    string? Artist,
    string? Album,
    string? Genre,
    int? Year,
    int? TrackNumber,
    int? DurationMs,
    string? Container,
    string? VideoCodec,
    string? AudioCodec,
    int? Width,
    int? Height);
