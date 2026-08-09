namespace Velnix.Core.Models;

/// <summary>
/// Immutable metadata describing an opened media source, as produced by
/// Playback's <c>IMediaDecoder</c> and consumed by Rendering, Audio,
/// Library, and UI. Kept in Core since every playback-adjacent module
/// depends on this shape.
/// </summary>
/// <param name="SourcePath">Path or URI of the media source.</param>
/// <param name="Duration">Total duration, or <c>null</c> if unknown (e.g. live stream).</param>
/// <param name="HasVideo">Whether the source contains at least one video stream.</param>
/// <param name="HasAudio">Whether the source contains at least one audio stream.</param>
/// <param name="Container">Container format identifier (e.g. "mkv", "mp4"), as reported by FFmpeg.</param>
public sealed record MediaInfo(
    string SourcePath,
    TimeSpan? Duration,
    bool HasVideo,
    bool HasAudio,
    string Container);

/// <summary>
/// A position within an open media source. Kept as a distinct type
/// (rather than a raw <see cref="TimeSpan"/>) so seek requests and
/// reported positions are not accidentally interchangeable with unrelated
/// durations elsewhere in the codebase.
/// </summary>
/// <param name="Offset">Offset from the start of the media.</param>
public readonly record struct PlaybackPosition(TimeSpan Offset)
{
    public static PlaybackPosition Zero => new(TimeSpan.Zero);
}
