namespace Velnix.MediaLibrary.Models;

public sealed class HistoryEntry
{
    public long Id { get; init; }
    public long TrackId { get; init; }
    public DateTime PlayedUtc { get; init; }
    public int LastPositionMs { get; init; }
    public bool Completed { get; init; }
}

public sealed class FavoriteEntry
{
    public long TrackId { get; init; }
    public DateTime AddedUtc { get; init; }
}
