#include "Logger.h"

#include <chrono>

namespace velnix::core
{

static u64 GetTimestamp()
{
    return static_cast<u64>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());
}

Logger::Logger() = default;

Logger::Logger(const String& logFile)
{
    m_file = std::make_unique<FileSink>(logFile);
}

void Logger::Log(
    LogLevel level,
    const String& category,
    const String& message)
{
    LogMessage log;

    log.Level = level;
    log.Category = category;
    log.Message = message;
    log.Timestamp = GetTimestamp();

    m_console.Write(log);

    if (m_file)
        m_file->Write(log);
}

void Logger::Trace(const String& c, const String& m)
{
    Log(LogLevel::Trace, c, m);
}

void Logger::Debug(const String& c, const String& m)
{
    Log(LogLevel::Debug, c, m);
}

void Logger::Info(const String& c, const String& m)
{
    Log(LogLevel::Info, c, m);
}

void Logger::Warning(const String& c, const String& m)
{
    Log(LogLevel::Warning, c, m);
}

void Logger::Error(const String& c, const String& m)
{
    Log(LogLevel::Error, c, m);
}

void Logger::Critical(const String& c, const String& m)
{
    Log(LogLevel::Critical, c, m);
}

}