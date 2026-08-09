namespace Velnix.Core.Exceptions;

/// <summary>
/// Base type for all Velnix-specific exceptions. Distinguishes expected,
/// recoverable failures (bad file, network unavailable) from true bugs,
/// per Coding_Standards.md § Error Handling.
/// </summary>
public abstract class VelnixException : Exception
{
    /// <summary>
    /// True if this failure is expected/recoverable (e.g. malformed input)
    /// and should surface as a graceful UI state rather than a crash.
    /// False indicates a genuine bug that should throw/log loudly in debug
    /// builds per Coding_Standards.md § Error Handling.
    /// </summary>
    public bool IsRecoverable { get; }

    protected VelnixException(string message, bool isRecoverable, Exception? innerException = null)
        : base(message, innerException)
    {
        IsRecoverable = isRecoverable;
    }
}
