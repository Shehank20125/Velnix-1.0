namespace Velnix.Audio;

/// <summary>
/// Placeholder <see cref="IAudioOutput"/> — accepts samples and discards
/// them. Same swap-later pattern as <c>Rendering.NullVideoRenderer</c>;
/// see that type's doc-comment for why. A real implementation needs
/// WASAPI (matches Constitution v1.1's Windows-only v1.x scope) via
/// <c>Windows.Media.Audio</c>/<c>AudioGraph</c> or raw WASAPI COM
/// interop — again a distinct, substantial chunk of work from decoding,
/// not rushed alongside it here.
/// </summary>
public sealed class NullAudioOutput : IAudioOutput
{
    public float Volume { get; set; } = 1.0f;
    public bool IsMuted { get; set; }
    public long SamplesWritten { get; private set; }

    public void WriteSamples(ReadOnlyMemory<float> samples)
    {
        SamplesWritten += samples.Length;
    }

    public void Flush()
    {
    }
}
