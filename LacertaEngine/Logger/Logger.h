#pragma once
#include "../Core.h"

enum class LogType
{
    Debug,
    Warning,
    Error
};

class Logger
{
public:
    void Log(LogType type, const std::string& content);

    void WriteLogsToFile();
    
    static Logger* Get();

private:
    std::vector<std::string> m_logMessages;
};

#define LOG(type, content) Logger::Get()->Log(LogType::type, content)