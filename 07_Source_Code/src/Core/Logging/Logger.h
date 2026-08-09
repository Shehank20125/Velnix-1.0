#pragma once

/**
 * @file Logger.h
 */

#include <memory>

#include "ConsoleSink.h"
#include "FileSink.h"

namespace velnix::core
{

class Logger final
{
public:

    Logger();

    explicit Logger(const String& logFile);

    void Trace(const String& category, const String& message);

    void Debug(const String& category, const String& message);

    void Info(const String& category, const String& message);

    void Warning(const String& category, const String& message);

    void Error(const String& category, const String& message);

    void Critical(const String& category, const String& message);

private:

    void Log(
        LogLevel level,
        const String& category,
        const String& message);

private:

    ConsoleSink m_console;

    std::unique_ptr<FileSink> m_file;

};

}