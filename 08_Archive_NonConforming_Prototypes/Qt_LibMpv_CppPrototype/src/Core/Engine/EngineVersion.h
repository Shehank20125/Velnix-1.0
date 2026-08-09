#pragma once

/**
 * @file EngineVersion.h
 * @brief Defines the current version of the Velnix Core Engine.
 */

#include <string_view>

namespace velnix::core
{
    struct EngineVersion final
    {
        static constexpr int Major = 0;
        static constexpr int Minor = 1;
        static constexpr int Patch = 0;

        static constexpr std::string_view Name = "Velnix Core Engine";
        static constexpr std::string_view Version = "0.1.0";
    };
}