using Velnix.Core.Exceptions;
using Velnix.DecodeHost;
using Velnix.Playback;

// ---------------------------------------------------------------------
// Velnix.DecodeHarness — throwaway verification tool, not shipped.
//
// Purpose (per src/DecodeHost/NOTES.md "Suggested first real session"):
//   1. Open a known-good media file through the real FFmpegMediaDecoder
//      and dump MediaInfo + the first few frame timestamps.
//   2. Optionally, run the same decoder against a deliberately malformed
//      file to confirm the hostile-input path throws MediaParsingException
//      cleanly instead of crashing (Threat_Model.md item 1).
//
// This intentionally bypasses WinUI, DI, and CompositionRoot — it is the
// cheapest possible surface for catching FFmpeg.AutoGen struct/enum
// mismatches (channel-layout handling, pixel format, etc.) before any of
// that complexity is in the picture.
//
// Usage:
//   Velnix.DecodeHarness.exe <path-to-media-file> [--frames N]
//   Velnix.DecodeHarness.exe --malformed
//
// Requires an LGPL FFmpeg shared build (avutil-*.dll, avcodec-*.dll,
// avformat-*.dll, swscale-*.dll, swresample-*.dll) in an "ffmpeg\"
// subfolder next to this exe's output — see DecodeHost/NOTES.md.
// ---------------------------------------------------------------------

const int DefaultFrameSampleCount = 5;

if (args.Length == 0)
{
    PrintUsage();
    return 1;
}

if (args[0].Equals("--malformed", StringComparison.OrdinalIgnoreCase))
{
    return await RunMalformedInputCheckAsync();
}

var sourcePath = args[0];
var frameSampleCount = DefaultFrameSampleCount;

for (var i = 1; i < args.Length; i++)
{
    if (args[i].Equals("--frames", StringComparison.OrdinalIgnoreCase) && i + 1 < args.Length
        && int.TryParse(args[i + 1], out var parsed))
    {
        frameSampleCount = parsed;
        i++;
    }
}

if (!File.Exists(sourcePath))
{
    Console.Error.WriteLine($"[FAIL] File not found: {sourcePath}");
    return 1;
}

return await RunOpenAndSampleAsync(sourcePath, frameSampleCount);

// -----------------------------------------------------------------
// Step 1: open a known-good file, print MediaInfo, sample frames.
// -----------------------------------------------------------------
static async Task<int> RunOpenAndSampleAsync(string sourcePath, int frameSampleCount)
{
    Console.WriteLine($"== Velnix.DecodeHarness == opening: {sourcePath}");
    Console.WriteLine($"Native FFmpeg search path: {Path.Combine(AppContext.BaseDirectory, "ffmpeg")}");
    Console.WriteLine();

    IMediaDecoder decoder = new FFmpegMediaDecoder();
    var stopwatch = System.Diagnostics.Stopwatch.StartNew();

    try
    {
        var info = await decoder.OpenAsync(sourcePath);
        stopwatch.Stop();

        Console.WriteLine("[OK] OpenAsync returned MediaInfo:");
        Console.WriteLine($"     SourcePath : {info.SourcePath}");
        Console.WriteLine($"     Container  : {info.Container}");
        Console.WriteLine($"     Duration   : {info.Duration?.ToString() ?? "(unknown)"}");
        Console.WriteLine($"     HasVideo   : {info.HasVideo}");
        Console.WriteLine($"     HasAudio   : {info.HasAudio}");
        Console.WriteLine($"     Open took  : {stopwatch.ElapsedMilliseconds} ms");
        Console.WriteLine();

        if (info.HasVideo)
        {
            await SampleFramesAsync(
                "video",
                frameSampleCount,
                () => decoder.ReadVideoFrameAsync(),
                frame => $"pts={frame.PresentationTimestamp,-14} {frame.Width}x{frame.Height} " +
                         $"data={frame.Data.Length,10} bytes (expect {frame.Width * frame.Height * 3})");
        }
        else
        {
            Console.WriteLine("[SKIP] No video stream — HasVideo was false.");
        }

        if (info.HasAudio)
        {
            await SampleFramesAsync(
                "audio",
                frameSampleCount,
                () => decoder.ReadAudioFrameAsync(),
                frame => $"pts={frame.PresentationTimestamp,-14} {frame.SampleRate} Hz " +
                         $"ch={frame.ChannelCount} samples={frame.Samples.Length}");
        }
        else
        {
            Console.WriteLine("[SKIP] No audio stream — HasAudio was false.");
        }

        await decoder.CloseAsync();
        Console.WriteLine();
        Console.WriteLine("[OK] CloseAsync completed without throwing.");
        return 0;
    }
    catch (MediaParsingException ex)
    {
        Console.Error.WriteLine($"[FAIL] MediaParsingException on a file expected to be valid: {ex.Message}");
        Console.Error.WriteLine($"       MediaSource: {ex.MediaSource}");
        if (ex.InnerException is not null)
        {
            Console.Error.WriteLine($"       Inner: {ex.InnerException}");
        }
        return 1;
    }
    catch (Exception ex)
    {
        // Anything landing here is exactly what NOTES.md warns about:
        // an uncaught crash instead of a clean MediaParsingException.
        // Treat this as the highest-priority bug to fix before anything
        // else — it means the hostile-input contract in Threat_Model.md
        // item 1 is not actually being honored.
        Console.Error.WriteLine("[CRASH] Unhandled exception — this should have been a MediaParsingException:");
        Console.Error.WriteLine(ex);
        return 2;
    }
}

static async Task SampleFramesAsync<TFrame>(
    string label,
    int count,
    Func<Task<TFrame?>> readNext,
    Func<TFrame, string> describe)
    where TFrame : class
{
    Console.WriteLine($"[{label}] reading first {count} frame(s):");
    for (var i = 0; i < count; i++)
    {
        var frame = await readNext();
        if (frame is null)
        {
            Console.WriteLine($"  [{label} #{i}] end of stream (null returned earlier than expected).");
            break;
        }

        Console.WriteLine($"  [{label} #{i}] {describe(frame)}");
    }

    Console.WriteLine();
}

// -----------------------------------------------------------------
// Step 2 (optional): confirm hostile input fails cleanly, not loudly.
// Per Threat_Model.md item 1 — every parsing path must treat input as
// hostile and throw MediaParsingException, never crash.
// -----------------------------------------------------------------
static async Task<int> RunMalformedInputCheckAsync()
{
    var tempPath = Path.Combine(Path.GetTempPath(), $"velnix-malformed-{Guid.NewGuid():N}.mp4");

    // Deliberately garbage: a plausible-looking size but no valid
    // container structure at all. This is the cheapest hostile-input
    // case; a real fuzz pass (truncated headers, corrupted atom sizes,
    // huge declared dimensions, etc.) belongs in Playback.Tests, not
    // here — this harness only sanity-checks the happy/crash boundary.
    var garbage = new byte[4096];
    new Random(12345).NextBytes(garbage);
    await File.WriteAllBytesAsync(tempPath, garbage);

    Console.WriteLine($"== Velnix.DecodeHarness == malformed-input check: {tempPath}");
    Console.WriteLine();

    IMediaDecoder decoder = new FFmpegMediaDecoder();

    try
    {
        var info = await decoder.OpenAsync(tempPath);
        Console.Error.WriteLine("[FAIL] OpenAsync succeeded against garbage input — expected MediaParsingException.");
        Console.Error.WriteLine($"       Returned MediaInfo: {info}");
        return 1;
    }
    catch (MediaParsingException ex)
    {
        Console.WriteLine("[OK] MediaParsingException thrown as expected:");
        Console.WriteLine($"     {ex.Message}");
        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine("[CRASH] Malformed input crashed instead of throwing MediaParsingException:");
        Console.Error.WriteLine(ex);
        return 2;
    }
    finally
    {
        try { File.Delete(tempPath); } catch { /* best-effort cleanup */ }
    }
}

static void PrintUsage()
{
    Console.WriteLine("Velnix.DecodeHarness — throwaway FFmpegMediaDecoder verification tool");
    Console.WriteLine();
    Console.WriteLine("Usage:");
    Console.WriteLine("  Velnix.DecodeHarness.exe <path-to-media-file> [--frames N]");
    Console.WriteLine("  Velnix.DecodeHarness.exe --malformed");
    Console.WriteLine();
    Console.WriteLine("Requires an LGPL FFmpeg shared build (avutil-*.dll, avcodec-*.dll,");
    Console.WriteLine("avformat-*.dll, swscale-*.dll, swresample-*.dll) in an 'ffmpeg\\'");
    Console.WriteLine("subfolder next to this exe. See src/DecodeHost/NOTES.md.");
}
