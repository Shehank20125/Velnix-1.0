#pragma once

/**
 * @file CommandType.h
 * @brief Defines all commands understood by the Media Engine.
 */

namespace velnix::core
{

enum class CommandType
{
    None = 0,

    //-------------------------
    // Engine
    //-------------------------

    Initialize,
    Shutdown,

    //-------------------------
    // Media
    //-------------------------

    Open,
    Close,

    //-------------------------
    // Playback
    //-------------------------

    Play,
    Pause,
    Stop,
    Seek,

    //-------------------------
    // Future
    //-------------------------

    NextFrame,
    PreviousFrame,

    SetVolume,
    SetPlaybackRate,

    SelectAudioTrack,
    SelectSubtitleTrack
};

}