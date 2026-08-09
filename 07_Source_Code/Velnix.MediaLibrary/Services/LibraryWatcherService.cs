using Velnix.MediaLibrary.Data.Repositories;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Services;

/// <summary>
/// Owns real-time monitoring for all configured library roots.
///
/// - Local paths: a FileSystemWatcher triggers a debounced rescan of that
///   root when files change.
/// - Network (UNC) paths: FileSystemWatcher is known to be unreliable over
///   SMB, so these are never watched directly — instead a per-root timer
///   fires ScanRoot on LibraryRoot.RescanIntervalMinutes.
///
/// This class is host-lifecycle-bound: call StartAsync once at app
/// startup after the library roots are loaded, and Dispose on shutdown.
/// </summary>
public sealed class LibraryWatcherService : IDisposable
{
    // Coalesces bursts of filesystem events (e.g. copying a folder of 200
    // files) into a single rescan instead of one per event.
    private static readonly TimeSpan DebounceWindow = TimeSpan.FromSeconds(3);

    private readonly LibraryRootRepository _roots;
    private readonly LibraryScanService _scanner;
    private readonly Dictionary<long, FileSystemWatcher> _watchers = new();
    private readonly Dictionary<long, Timer> _debounceTimers = new();
    private readonly Dictionary<long, Timer> _networkRescanTimers = new();
    private readonly object _lock = new();

    public event Action<long, ScanResult>? RootScanned;

    public LibraryWatcherService(LibraryRootRepository roots, LibraryScanService scanner)
    {
        _roots = roots;
        _scanner = scanner;
    }

    public void Start()
    {
        foreach (var root in _roots.GetAll().Where(r => r.Enabled))
        {
            StartWatchingRoot(root);
            // Baseline scan on startup for every root, local or network.
            RunScan(root);
        }
    }

    private void StartWatchingRoot(LibraryRoot root)
    {
        if (root.IsNetworkPath)
        {
            var interval = TimeSpan.FromMinutes(Math.Max(1, root.RescanIntervalMinutes));
            var timer = new Timer(_ => RunScan(root), null, interval, interval);
            lock (_lock) { _networkRescanTimers[root.Id] = timer; }
            return;
        }

        if (!Directory.Exists(root.Path))
        {
            // Local path missing at startup (e.g. removable drive not
            // connected) — skip watcher setup; a manual/periodic rescan
            // from the host UI can retry once the path exists again.
            return;
        }

        var watcher = new FileSystemWatcher(root.Path)
        {
            IncludeSubdirectories = true,
            NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite | NotifyFilters.Size,
            EnableRaisingEvents = true,
        };

        void OnChanged(object sender, FileSystemEventArgs e) => Debounce(root);
        void OnRenamed(object sender, RenamedEventArgs e) => Debounce(root);

        watcher.Created += OnChanged;
        watcher.Changed += OnChanged;
        watcher.Deleted += OnChanged;
        watcher.Renamed += OnRenamed;
        watcher.Error += (_, e) =>
        {
            // Watcher buffer overflow or handle failure: fall back to a
            // one-off rescan so we don't silently stop tracking this root.
            RunScan(root);
        };

        lock (_lock) { _watchers[root.Id] = watcher; }
    }

    private void Debounce(LibraryRoot root)
    {
        lock (_lock)
        {
            if (_debounceTimers.TryGetValue(root.Id, out var existing))
            {
                existing.Change(DebounceWindow, Timeout.InfiniteTimeSpan);
                return;
            }

            var timer = new Timer(_ => RunScan(root), null, DebounceWindow, Timeout.InfiniteTimeSpan);
            _debounceTimers[root.Id] = timer;
        }
    }

    private void RunScan(LibraryRoot root)
    {
        var result = _scanner.ScanRoot(root);
        RootScanned?.Invoke(root.Id, result);
    }

    public void Dispose()
    {
        lock (_lock)
        {
            foreach (var w in _watchers.Values) w.Dispose();
            foreach (var t in _debounceTimers.Values) t.Dispose();
            foreach (var t in _networkRescanTimers.Values) t.Dispose();
            _watchers.Clear();
            _debounceTimers.Clear();
            _networkRescanTimers.Clear();
        }
    }
}
