namespace Velnix.Diagnostics;

/// <summary>
/// No-op logger. This is the default binding until the user opts in to
/// diagnostics logging (Constitution v1.1 § Privacy Policy: "no hidden
/// telemetry ... user control over privacy"). A real file-based or
/// in-memory-ring-buffer sink should be implemented and swapped in via DI
/// once the Settings module can express the opt-in toggle.
/// </summary>
public sealed class NullVelnixLogger : IVelnixLogger
{
    public void LogDebug(string message) { }

    public void LogInfo(string message) { }

    public void LogRecoverable(string message, Exception? exception = null) { }

    public void LogFault(string message, Exception? exception = null) { }
}
