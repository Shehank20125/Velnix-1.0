namespace Velnix.Core.Enums;

/// <summary>
/// High-level playback state, shared across Playback, Rendering, Audio,
/// and UI so those modules don't each define their own competing enum.
/// Lives in Core because it has no dependencies on other modules.
/// </summary>
public enum PlaybackState
{
    Idle,
    Opening,
    Playing,
    Paused,
    Seeking,
    Buffering,
    Ended,
    Faulted,
}
