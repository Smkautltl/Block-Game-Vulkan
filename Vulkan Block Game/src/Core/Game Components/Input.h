#pragma once
#include <map>

#include "Core/Vulkan/Window.h"

enum PRESSTYPE
{
	RELEASE = 0,
	PRESS = 1,
	REPEAT = 2

};

typedef std::pair<int, int> KeyBind;
extern std::map<int, bool> KeyPressed;

class Input
{
public:
	void SetKeyCallback(Window& window);

private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:

private:
   
};