#pragma once
#include "Core/VulkanRender.h"

class Vulkan
{
public:
	void run();

private:
	void init_window();
	void init_vulkan();
	void main_loop();

public:

private:
	VulkanRender Renderer;

	GLFWwindow* window_{};

	VkDevice device_{};
};
