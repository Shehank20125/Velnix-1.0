#pragma once

/**
 * @file EngineError.h
 * @brief Defines standardized error codes for the Velnix Media Engine.
 *
 * Every subsystem returns one of these error codes through EngineResult.
 * Error messages should be generated separately, allowing localization
 * and consistent reporting.
 */

#include <cstdint>

namespace velnix::core
{
    /**
     * @brief Standard error codes used throughout the engine.
     */
    enum class EngineError : std::uint32_t
    {
        None = 0,

        //-------------------------------------------------------------------------
        // Engine
        //-------------------------------------------------------------------------

        AlreadyInitialized,
        NotInitialized,
        InvalidState,
        OperationCancelled,

        //-------------------------------------------------------------------------
        // Media
        //-------------------------------------------------------------------------

        FileNotFound,
        FileAccessDenied,
        InvalidMedia,
        UnsupportedContainer,
        UnsupportedCodec,
        CorruptedMedia,

        //-------------------------------------------------------------------------
        // Playback
        //-------------------------------------------------------------------------

        PlaybackFailed,
        InvalidSeekPosition,
        EndOfFile,

        //-------------------------------------------------------------------------
        // Decoder
        //-------------------------------------------------------------------------

        DecoderInitializationFailed,
        DecoderUnavailable,
        DecodeFailed,

        //-------------------------------------------------------------------------
        // Renderer
        //-------------------------------------------------------------------------

        RendererInitializationFailed,
        RendererUnavailable,
        RendererDeviceLost,

        //-------------------------------------------------------------------------
        // Audio
        //-------------------------------------------------------------------------

        AudioInitializationFailed,
        AudioDeviceUnavailable,
        AudioOutputFailed,

        //-------------------------------------------------------------------------
        // Hardware Acceleration
        //-------------------------------------------------------------------------

        HardwareAccelerationUnavailable,
        HardwareDeviceCreationFailed,

        //-------------------------------------------------------------------------
        // IPC
        //-------------------------------------------------------------------------

        IPCConnectionFailed,
        IPCSendFailed,
        IPCReceiveFailed,
        IPCTimeout,

        //-------------------------------------------------------------------------
        // Threading
        //-------------------------------------------------------------------------

        ThreadCreationFailed,
        ThreadShutdownFailed,

        //-------------------------------------------------------------------------
        // Memory
        //-------------------------------------------------------------------------

        OutOfMemory,

        //-------------------------------------------------------------------------
        // Configuration
        //-------------------------------------------------------------------------

        ConfigurationError,

        //-------------------------------------------------------------------------
        // Unknown
        //-------------------------------------------------------------------------

        Unknown
    };
}