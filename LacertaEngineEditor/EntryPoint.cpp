#include <iostream>
#include <LacertaEngine.h>

#include "EditorWindow/EditorWindow.h"
#include "InputSystem/InputSystem.h"
#include "Window/Window.h"

// Lacerta Engine Editor Entry Point
// Launches the editor and uses LacertaEngine DLL core code

int main(int argc, char* argv[])
{
    std::cout << "Hello There !" << std::endl;
    LOG(Debug, "Hello Log");

    LacertaEngineEditor::EditorWindow EngineWindow(L"EngineWindow");
    LacertaEngine::InputSystem::Create();
    LacertaEngine::InputSystem::Get()->AddListener(&EngineWindow);
    
    while(EngineWindow.IsRunning())
    {
        LacertaEngine::InputSystem::Get()->Update();
    }

    LacertaEngine::InputSystem::Release();

    LacertaEngine::Logger::Get()->WriteLogsToFile();

    std::cin.get();

    return 0;
}