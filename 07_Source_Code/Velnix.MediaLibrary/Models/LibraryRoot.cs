namespace Velnix.MediaLibrary.Models;

/// <summary>
/// A configured library folder. Local paths get a live FileSystemWatcher.
/// Network (UNC) paths do not — FileSystemWatcher is unreliable over SMB —
/// so they instead get a scheduled rescan every RescanIntervalMinutes.
/// </summary>
public sealed class LibraryRoot
{
    public long Id { get; init; }
    public required string Path { get; init; }
    public bool IsNetworkPath { get; init; }
    public bool Enabled { get; init; } = true;
    public DateTime? LastScannedUtc { get; init; }
    public int RescanIntervalMinutes { get; init; } = 15;
}
