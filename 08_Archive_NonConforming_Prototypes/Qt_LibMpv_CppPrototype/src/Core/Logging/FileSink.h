#pragma once

/**
 * @file FileSink.h
 */

#include <fstream>

#include "LogMessage.h"

namespace velnix::core
{

class FileSink
{
public:

    explicit FileSink(const String& file);

    ~FileSink();

    void Write(const LogMessage& message);

private:

    std::ofstream m_stream;

};

}