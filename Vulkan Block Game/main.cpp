#include "App.h"
#include <iostream>

Player* playerInstance = nullptr;
std::map<int, bool> KeyPressed{};
App Application{};

int main() 
{
    try 
    {
        Application.Start();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}