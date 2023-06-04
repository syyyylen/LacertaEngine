#include <iostream>
#include <LacertaEngine.h>

#include "LacertaEditor.h"

// Lacerta Engine Editor Entry Point
// Launches the editor and uses LacertaEngine DLL core code

int main(int argc, char* argv[])
{
    std::cout << "Hello There !" << std::endl;

    LacertaEngineEditor::LacertaEditor LacertaEditor;
    LacertaEditor.Start();
    while(LacertaEditor.IsRunning())
    {
        LacertaEditor.Update();
    }
    LacertaEditor.Quit();

    std::cin.get();
    return 0;
}