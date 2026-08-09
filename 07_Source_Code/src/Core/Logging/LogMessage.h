#pragma once

/**
 * @file LogMessage.h
 */

#include "../Common/Types.h"
#include "LogLevel.h"

namespace velnix::core
{

struct LogMessage
{
    LogLevel Level = LogLevel::Info;

    String Category;

    String Message;

    u64 Timestamp = 0;
};

}