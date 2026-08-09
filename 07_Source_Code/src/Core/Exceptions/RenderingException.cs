namespace Velnix.Core.Exceptions;

/// <summary>
/// Thrown when the presentation surface (D3D11 device, swapchain, or the
/// WinUI interop binding to it) fails. Unlike <see cref="MediaParsingException"/>,
/// this is NOT hostile-input territory — it's driver/GPU/OS-surface
/// failure (device removed, no compatible adapter, swapchain creation
/// rejected). Marked recoverable: <c>Rendering.IVideoRenderer</c>
/// implementations should throw this rather than crash, so the UI layer
/// can fall back to an audio-only or "video unavailable" state instead of
/// taking the whole app down over a graphics driver hiccup.
/// </summary>
public sealed class RenderingException : VelnixException
{
    public RenderingException(string message, Exception? innerException = null)
        : base(message, isRecoverable: true, innerException)
    {
    }
}
