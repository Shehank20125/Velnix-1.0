#pragma once

/**
 * @file Types.h
 * @brief Common type aliases used throughout the Velnix Core Engine.
 *
 * This header contains lightweight aliases and utility types that are shared
 * across all engine modules. It should remain free of implementation logic.
 */

#include <cstdint>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace velnix::core
{
    //--------------------------------------------------------------------------
    // Integer Types
    //--------------------------------------------------------------------------

    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    //--------------------------------------------------------------------------
    // Floating Point Types
    //--------------------------------------------------------------------------

    using f32 = float;
    using f64 = double;

    //--------------------------------------------------------------------------
    // String Types
    //--------------------------------------------------------------------------

    using String     = std::string;
    using StringView = std::string_view;

    //--------------------------------------------------------------------------
    // File System Types
    //--------------------------------------------------------------------------

    using Path = std::filesystem::path;

    //--------------------------------------------------------------------------
    // Time Types
    //--------------------------------------------------------------------------

    using Clock       = std::chrono::steady_clock;
    using TimePoint   = Clock::time_point;
    using Milliseconds = std::chrono::milliseconds;
    using Seconds      = std::chrono::seconds;

    //--------------------------------------------------------------------------
    // Smart Pointer Aliases
    //--------------------------------------------------------------------------

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
}