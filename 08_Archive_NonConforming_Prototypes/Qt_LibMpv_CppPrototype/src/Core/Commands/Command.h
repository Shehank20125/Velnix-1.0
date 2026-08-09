#pragma once

#include "CommandType.h"

#include "../Common/Types.h"

namespace velnix::core
{

struct Command
{
    CommandType Type = CommandType::None;

    Path File{};

    Milliseconds Position{0};
};

}