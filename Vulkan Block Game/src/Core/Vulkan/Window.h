#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

#include "Log.h"

static uint32_t WINWIDTH = 1280;
static uint32_t WINHEIGHT = 720;

class Window
{
public:
	Window(int w, int h, std::string name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	GLFWwindow* GetWindow() { return window; }

	bool ShouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D GetExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

	bool WasWindowResized() { return frameBufferResized; }
	void ResetWindowResizedFlag() { frameBufferResized = false; }

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	void SaveWindowData(std::vector<int> windowPosSize) { WindowedPositionSize = windowPosSize; }
	std::vector<int> GetWindowData() { return WindowedPositionSize; }

private:
	void initWindow();
	//static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

public:
	int width;
	int height;
	bool frameBufferResized = false;

	std::vector<int> WindowedPositionSize;

private:
	std::string windowName;
	GLFWwindow* window;
};