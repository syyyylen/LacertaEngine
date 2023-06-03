#include <iostream>
#include <LacertaEngine.h>

#include "Window/Window.h"

// Lacerta Engine Editor Entry Point
// Launches the editor and uses LacertaEngine DLL core code

int main(int argc, char* argv[])
{
    std::cout << "Hello There !" << std::endl;
    LOG(Debug, "Hello Log");

    LacertaEngine::Window EngineWindow(L"EngineWindow");
    while(EngineWindow.IsRunning());

    LacertaEngine::Logger::Get()->WriteLogsToFile();

    std::cin.get();

    return 0;
}