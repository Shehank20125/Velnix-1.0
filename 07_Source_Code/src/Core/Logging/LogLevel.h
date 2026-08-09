#pragma once

/**
 * @file LogLevel.h
 * @brief Logging severity levels.
 */

namespace velnix::core
{

enum class LogLevel
{
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

}