#include "ConsoleSink.h"

#include <iostream>

namespace velnix::core
{

void ConsoleSink::Write(const LogMessage& message)
{
    std::cout
        << "[" << static_cast<int>(message.Level) << "] "
        << "[" << message.Category << "] "
        << message.Message
        << std::endl;
}

}