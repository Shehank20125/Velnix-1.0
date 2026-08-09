using Velnix.MediaLibrary.Data.Repositories;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Services;

/// <summary>
/// Walks a library root and reconciles the tracks table with what's on
/// disk: new files are inserted, changed files are re-probed, files no
/// longer present are marked missing (not deleted — history/favorites/
/// playlists referencing them should survive a temporarily-unplugged
/// drive or offline network share).
/// </summary>
public sealed class LibraryScanService
{
    // Extend as more containers are supported by the playback engine.
    private static readonly HashSet<string> SupportedExtensions = new(StringComparer.OrdinalIgnoreCase)
    {
        ".mp4", ".mkv", ".avi", ".mov", ".webm", ".mp3", ".flac", ".wav", ".m4a", ".ogg"
    };

    private readonly TrackRepository _tracks;
    private readonly LibraryRootRepository _roots;
    private readonly IMediaMetadataProbe _probe;

    public LibraryScanService(
        TrackRepository tracks,
        LibraryRootRepository roots,
        IMediaMetadataProbe probe)
    {
        _tracks = tracks;
        _roots = roots;
        _probe = probe;
    }

    /// <summary>Full scan of a single root. Safe to call repeatedly (idempotent upserts).</summary>
    public ScanResult ScanRoot(LibraryRoot root, CancellationToken ct = default)
    {
        int added = 0, updated = 0, errors = 0;
        var presentPaths = new List<string>();

        if (!Directory.Exists(root.Path))
        {
            // Root unreachable (e.g. network share offline). Don't mark
            // everything missing on a transient outage — just skip this pass.
            return new ScanResult(0, 0, 0, RootReachable: false);
        }

        IEnumerable<string> files;
        try
        {
            files = Directory.EnumerateFiles(root.Path, "*", SearchOption.AllDirectories)
                .Where(f => SupportedExtensions.Contains(Path.GetExtension(f)));
        }
        catch (UnauthorizedAccessException)
        {
            return new ScanResult(0, 0, 0, RootReachable: false);
        }

        foreach (var filePath in files)
        {
            ct.ThrowIfCancellationRequested();
            presentPaths.Add(filePath);

            try
            {
                var isNew = !_tracks.ExistsByPath(filePath);
                var info = new FileInfo(filePath);
                var probed = SafeProbe(filePath);

                var track = new Track
                {
                    FilePath = filePath,
                    FileSizeBytes = info.Length,
                    LastModifiedUtc = info.LastWriteTimeUtc,
                    SourceRootId = root.Id,
                    Title = probed?.Title,
                    Artist = probed?.Artist,
                    Album = probed?.Album,
                    Genre = probed?.Genre,
                    Year = probed?.Year,
                    TrackNumber = probed?.TrackNumber,
                    DurationMs = probed?.DurationMs,
                    Container = probed?.Container ?? Path.GetExtension(filePath).TrimStart('.'),
                    VideoCodec = probed?.VideoCodec,
                    AudioCodec = probed?.AudioCodec,
                    Width = probed?.Width,
                    Height = probed?.Height,
                    AddedUtc = DateTime.UtcNow,
                };

                _tracks.Upsert(track);
                if (isNew) added++; else updated++;
            }
            catch (IOException)
            {
                // File in use / transient read error — skip this pass, it
                // will be picked up on the next scan or watcher event.
                errors++;
            }
        }

        _tracks.MarkMissingExcept(root.Id, presentPaths);
        _roots.SetLastScanned(root.Id, DateTime.UtcNow);

        return new ScanResult(added, updated, errors, RootReachable: true);
    }

    private ProbedMetadata? SafeProbe(string filePath)
    {
        try
        {
            return _probe.Probe(filePath);
        }
        catch
        {
            // Corrupt/unreadable media must not abort the whole scan.
            return null;
        }
    }
}

public readonly record struct ScanResult(int Added, int Updated, int Errors, bool RootReachable);
