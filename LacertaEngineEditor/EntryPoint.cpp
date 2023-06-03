#include <iostream>
#include <LacertaEngine.h>

#include "Window/Window.h"

// Lacerta Engine Editor Entry Point
// Launches the editor and uses LacertaEngine DLL core code

int main(int argc, char* argv[])
{
    std::cout << "Hello There !" << std::endl;

    LOG(Debug, "This is a debug message !");
    LOG(Warning, "This is a warning message !");
    LOG(Error, "This is a error message !");

    LacertaEngine::Window EngineWindow(L"EngineWindow");

    LacertaEngine::Logger::Get()->WriteLogsToFile();

    std::cin.get();

    return 0;
}