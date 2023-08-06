#pragma once
#include "../Core.h"

namespace LacertaEngine {

enum class LogType
{
    Debug,
    Warning,
    Error
};

class LACERTAENGINE_API Logger
{
public:
    void Log(LogType type, const std::string& content);

    void WriteLogsToFile();

    static Logger* Get();

private:
    std::vector<std::string> m_logMessages;
};

#define LOG(type, content) LacertaEngine::Logger::Get()->Log(LacertaEngine::LogType::type, content)

#define STARTTIME() 

}