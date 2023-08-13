
#include "Window.h"

Window::Window(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } 
{
    Log::Init();
    initWindow();
}
Window::~Window() 
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

//Public
void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to create window surface!");
    }
}

//Private
void Window::initWindow() 
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}