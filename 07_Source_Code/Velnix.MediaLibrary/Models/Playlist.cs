namespace Velnix.MediaLibrary.Models;

public sealed class Playlist
{
    public long Id { get; init; }
    public required string Name { get; init; }
    public DateTime CreatedUtc { get; init; }
    public DateTime UpdatedUtc { get; init; }
}

public sealed class PlaylistItem
{
    public long Id { get; init; }
    public long PlaylistId { get; init; }
    public long TrackId { get; init; }
    public int SortOrder { get; init; }
}
