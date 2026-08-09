#pragma once

/**
 * @file EngineState.h
 * @brief Defines the lifecycle states of the Velnix Media Engine.
 *
 * The engine is always in exactly one state.
 * State transitions are validated by MediaEngine before executing commands.
 */

namespace velnix::core
{
    /**
     * @brief Represents the current lifecycle state of the media engine.
     */
    enum class EngineState
    {
        /// Engine object created but not initialized.
        Uninitialized = 0,

        /// Initializing internal subsystems.
        Initializing,

        /// Ready to open media.
        Idle,

        /// Opening media and preparing playback.
        Loading,

        /// Media loaded and ready.
        Ready,

        /// Currently playing media.
        Playing,

        /// Playback temporarily paused.
        Paused,

        /// Seeking to a different playback position.
        Seeking,

        /// Waiting for decoder or network data.
        Buffering,

        /// Playback has stopped.
        Stopped,

        /// Closing the current media session.
        Closing,

        /// Engine is shutting down.
        Shutdown
    };
}