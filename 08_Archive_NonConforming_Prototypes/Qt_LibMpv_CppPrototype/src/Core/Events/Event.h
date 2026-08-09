#pragma once

/**
 * @file Event.h
 */

#include "../Common/Types.h"
#include "EventType.h"

namespace velnix::core
{

struct Event
{
    EventType Type = EventType::None;

    u64 Timestamp = 0;

    String Message;
};

}