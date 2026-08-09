namespace Velnix.Core.Exceptions;

/// <summary>
/// Thrown when a subtitle file fails to parse or triggers unsafe behavior.
/// Per Threat_Model.md item 2, subtitle formats (especially SSA/ASS via
/// libass, which support embedded scripting/styling) are treated with the
/// same suspicion as media containers. No subtitle-triggered code
/// execution or unbounded resource use is acceptable — this exception
/// exists as the recoverable escape hatch when that boundary is hit.
/// </summary>
public sealed class SubtitleParsingException : VelnixException
{
    /// <summary>Path or identifier of the subtitle file that failed to parse.</summary>
    public string SubtitleSource { get; }

    public SubtitleParsingException(string subtitleSource, string message, Exception? innerException = null)
        : base(message, isRecoverable: true, innerException)
    {
        SubtitleSource = subtitleSource;
    }
}
