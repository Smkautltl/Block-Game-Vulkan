#include "Input.h"

void Input::SetKeyCallback(Window& window)
{
	glfwSetKeyCallback(window.GetWindow(), keyCallback);
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	KeyPressed[key] = action;
}