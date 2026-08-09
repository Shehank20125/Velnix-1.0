#include "FileSink.h"

namespace velnix::core
{

FileSink::FileSink(const String& file)
{
    m_stream.open(file, std::ios::app);
}

FileSink::~FileSink()
{
    if (m_stream.is_open())
        m_stream.close();
}

void FileSink::Write(const LogMessage& message)
{
    if (!m_stream.is_open())
        return;

    m_stream
        << "[" << static_cast<int>(message.Level) << "] "
        << "[" << message.Category << "] "
        << message.Message
        << std::endl;
}

}