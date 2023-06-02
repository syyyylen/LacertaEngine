#include <iostream>
#include "Core.h"
#include "Logger/Logger.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello There !" << std::endl;

    LOG(Debug, "This is a debug message !");
    LOG(Warning, "This is a warning message !");
    LOG(Error, "This is a error message !");

    Logger::Get()->WriteLogsToFile();

    std::cin.get();
    
    return 0;
}
