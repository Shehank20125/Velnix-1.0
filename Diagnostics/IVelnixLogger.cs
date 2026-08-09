namespace Velnix.Diagnostics;

/// <summary>
/// Logging boundary used by every other module for error handling per
/// Coding_Standards.md § Error Handling ("never swallow exceptions
/// silently — at minimum, log"). Per Constitution v1.1's Privacy Policy,
/// logging is opt-in and strictly local — no implementation of this
/// interface may transmit log data off-device.
/// </summary>
public interface IVelnixLogger
{
    void LogDebug(string message);

    void LogInfo(string message);

    /// <summary>
    /// Logs an expected, recoverable failure (bad file, network
    /// unavailable) — distinct from <see cref="LogFault"/> for true bugs,
    /// per Coding_Standards.md § Error Handling.
    /// </summary>
    void LogRecoverable(string message, Exception? exception = null);

    /// <summary>
    /// Logs a true bug / unexpected fault. Should be loud in debug builds
    /// per Coding_Standards.md § Error Handling.
    /// </summary>
    void LogFault(string message, Exception? exception = null);
}
