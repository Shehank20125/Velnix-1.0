namespace Velnix.MediaLibrary.Models;

// Owned by the Phase 5 SQLite schema per the Phase 4/5 coordination note —
// bookmarks and chapter caching are not a separate store.

public sealed class Bookmark
{
    public long Id { get; init; }
    public long TrackId { get; init; }
    public int PositionMs { get; init; }
    public string? Label { get; init; }
    public DateTime CreatedUtc { get; init; }
}

public sealed class ChapterCacheEntry
{
    public long Id { get; init; }
    public long TrackId { get; init; }
    public int StartMs { get; init; }
    public string? Title { get; init; }
}
