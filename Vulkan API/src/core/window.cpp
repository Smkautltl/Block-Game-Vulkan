#include "Window.h"

//-=-=-=-=- STD -=-=-=-=-
#include <stdexcept>
#include <utility>

namespace Vulkan
{
	Window::Window()
	{
		Log::Init();
		VK_CORE_TRACE("Log Started!")
		init_window();
	}
	Window::Window(int w, int h, std::string name) : WIDTH(w), HEIGHT(h), window_name_(std::move(name))
	{
		Log::Init();
		VK_CORE_TRACE("Log Started!")
		init_window();
	}

	Window::~Window()
	{
		glfwDestroyWindow(gl_window_);
		glfwTerminate();
	}

	void Window::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if(glfwCreateWindowSurface(instance, gl_window_, nullptr, surface) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to create window surface");
		}
	}

	void Window::init_window()
	{
		//Initializes the GLFW library
		glfwInit();
		VK_CORE_TRACE("Window Started!")

		//Tell GLFW not to use an API as we are using Vulkan
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Tells GLFW that the window is resizeable
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		//Creates a window
		gl_window_ = glfwCreateWindow(WIDTH, HEIGHT, window_name_.c_str(), nullptr, nullptr);

		//Sets up callback for when window is resized
		glfwSetWindowUserPointer(gl_window_, this);
		glfwSetFramebufferSizeCallback(gl_window_, framebuffer_resize_callback);
		glfwSetKeyCallback(gl_window_, key_callback);
	}

	void Window::framebuffer_resize_callback(GLFWwindow *gl_window, int width, int height)
	{
		//Gets the current window and assigns a new width and height
		auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(gl_window));
		window->frame_buffer_resized = true;
		window->WIDTH = width;
		window->HEIGHT = height;
	}

	void Window::key_callback(GLFWwindow* gl_window, int key, int scancode, int action, int mods)
	{
		//Gets the current window and assigns a new width and height
		auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(gl_window));
		window->key = key;
		window->scancode = scancode;
		window->action = action;
		window->mods = mods;
	}
}
