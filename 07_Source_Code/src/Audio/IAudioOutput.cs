namespace Velnix.Audio;

/// <summary>
/// Boundary interface for audio playback output. Separated from
/// Rendering (video) so audio/video sync logic in Playback can depend on
/// each independently, per Architecture_Overview.md's module boundaries.
/// </summary>
public interface IAudioOutput
{
    float Volume { get; set; }

    bool IsMuted { get; set; }

    void WriteSamples(ReadOnlyMemory<float> samples);

    void Flush();
}
