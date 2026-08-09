#pragma once

/**
 * @file EngineConfig.h
 * @brief Global configuration values for the Velnix Core Engine.
 */

#include "../Common/Types.h"

namespace velnix::core
{
    struct EngineConfig final
    {
        // Decoder
        bool EnableHardwareAcceleration = true;
        bool EnableSoftwareFallback = true;

        // Playback
        bool AutoPlay = false;
        bool LoopPlayback = false;

        // Rendering
        bool EnableVSync = true;

        // Logging
        bool EnableLogging = true;

        // Future
        Path CacheDirectory{};
        Path ConfigurationDirectory{};
    };
}