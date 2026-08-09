#pragma once

/**
 * @file ConsoleSink.h
 */

#include "LogMessage.h"

namespace velnix::core
{

class ConsoleSink
{
public:

    void Write(const LogMessage& message);

};

}