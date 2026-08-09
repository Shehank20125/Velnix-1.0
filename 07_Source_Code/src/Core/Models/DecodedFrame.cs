namespace Velnix.Core.Models;

/// <summary>
/// One decoded, presentation-ready video frame. <see cref="Data"/> is
/// always packed RGB24 (3 bytes/pixel, no padding) regardless of the
/// source pixel format — the decoder is responsible for the conversion
/// (e.g. via swscale) so <c>Rendering.IVideoRenderer</c> never needs to
/// know about YUV, NV12, or any other source layout.
/// </summary>
/// <param name="Data">Packed RGB24 pixel data, length == Width * Height * 3.</param>
/// <param name="Width">Frame width in pixels.</param>
/// <param name="Height">Frame height in pixels.</param>
/// <param name="PresentationTimestamp">
/// Presentation time relative to the start of the stream. The playback
/// controller uses this for A/V sync — never assume decode order equals
/// presentation order.
/// </param>
public sealed record DecodedVideoFrame(
    ReadOnlyMemory<byte> Data,
    int Width,
    int Height,
    TimeSpan PresentationTimestamp);

/// <summary>
/// One decoded audio frame. <see cref="Samples"/> is always interleaved
/// 32-bit float PCM regardless of source format — the decoder is
/// responsible for resampling/format conversion (e.g. via swresample) so
/// <c>Audio.IAudioOutput</c> only ever sees one format.
/// </summary>
/// <param name="Samples">Interleaved float PCM, length == FrameCount * ChannelCount.</param>
/// <param name="ChannelCount">Number of interleaved channels.</param>
/// <param name="SampleRate">Sample rate in Hz.</param>
/// <param name="PresentationTimestamp">Presentation time relative to the start of the stream.</param>
public sealed record DecodedAudioFrame(
    ReadOnlyMemory<float> Samples,
    int ChannelCount,
    int SampleRate,
    TimeSpan PresentationTimestamp);
