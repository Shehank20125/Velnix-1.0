namespace Velnix.Core.Exceptions;

/// <summary>
/// Thrown when a media file fails to parse or decode. Per Threat_Model.md
/// item 1, all media parsing/decoding paths treat input as hostile by
/// default — this exception is always recoverable and must be caught at
/// the module boundary and surfaced as "couldn't play this file," never
/// an unhandled crash.
/// </summary>
public sealed class MediaParsingException : VelnixException
{
    /// <summary>Path or identifier of the media file that failed to parse.</summary>
    public string MediaSource { get; }

    public MediaParsingException(string mediaSource, string message, Exception? innerException = null)
        : base(message, isRecoverable: true, innerException)
    {
        MediaSource = mediaSource;
    }
}
