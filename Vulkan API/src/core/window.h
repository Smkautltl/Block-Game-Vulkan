#pragma once

//-=-=-=-=- GLFW -=-=-=-=-
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//-=-=-=-=- CUSTOM -=-=-=-=-
#include "Log.h"

//-=-=-=-=- STD -=-=-=-=-

namespace Vulkan
{
	class Window
	{
	public:
		Window(int w, int h, std::string name);
		Window();
		~Window();

		//Deleting copy constructors
		Window(const Window&) = delete;
		Window &operator=(const Window&) = delete;

		bool should_close() { return glfwWindowShouldClose(gl_window_); }
		VkExtent2D get_extent() { return { static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT) }; }
		
		void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
		
		bool was_window_resized() { return frame_buffer_resized; }
		void reset_window_resize_flag() { frame_buffer_resized = false; }
	
	private:
		void init_window();
		static void framebuffer_resize_callback(GLFWwindow *gl_window, int width, int height);

	private:
		int WIDTH = 800;
		int HEIGHT = 600;
		bool frame_buffer_resized = false;
		
		std::string window_name_ = "Vulkan";
		GLFWwindow *gl_window_{};
	};
}
