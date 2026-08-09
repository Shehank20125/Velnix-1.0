#pragma once

/**
 * @file EventType.h
 * @brief Event identifiers used throughout the engine.
 */

namespace velnix::core
{

enum class EventType
{
    None = 0,

    //------------------------------------------------
    // Engine
    //------------------------------------------------

    EngineInitialized,
    EngineShutdown,

    //------------------------------------------------
    // Media
    //------------------------------------------------

    MediaOpened,
    MediaClosed,

    //------------------------------------------------
    // Playback
    //------------------------------------------------

    PlaybackStarted,
    PlaybackPaused,
    PlaybackStopped,

    SeekStarted,
    SeekCompleted,

    BufferingStarted,
    BufferingCompleted,

    EndOfStream,

    //------------------------------------------------
    // Decoder
    //------------------------------------------------

    DecoderStarted,
    DecoderStopped,

    //------------------------------------------------
    // Renderer
    //------------------------------------------------

    FramePresented,

    //------------------------------------------------
    // Audio
    //------------------------------------------------

    AudioStarted,
    AudioStopped,

    //------------------------------------------------
    // Errors
    //------------------------------------------------

    ErrorOccurred
};

}