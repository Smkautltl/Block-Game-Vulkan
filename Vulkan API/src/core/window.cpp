#include "Window.h"

//-=-=-=-=- STD -=-=-=-=-
#include <stdexcept>
#include <utility>

namespace Vulkan
{
	Window::Window()
	{
		Log::Init();
		VK_CORE_TRACE("Core Log Started!")
		init_window();
	}
	Window::Window(int w, int h, std::string name) : WIDTH(w), HEIGHT(h), window_name_(std::move(name))
	{
		Log::Init();
		VK_CORE_TRACE("Core Log Started!")
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
		VK_CORE_INFO("Window Surface Created!")
	}

	void Window::recentre_mouse()
	{
		SetCursorPos(WIDTH / 2, HEIGHT / 2);
	}

	void Window::init_window()
	{
		//Initializes the GLFW library
		glfwInit();

		//Tell GLFW not to use an API as we are using Vulkan
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Tells GLFW that the window is resizeable
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		//Creates a window
		gl_window_ = glfwCreateWindow(WIDTH, HEIGHT, window_name_.c_str(), nullptr, nullptr);
		VK_CORE_INFO("Window Created!")
		
		//Sets up callback for when window is resized
		glfwSetWindowUserPointer(gl_window_, this);
		glfwSetFramebufferSizeCallback(gl_window_, framebuffer_resize_callback);
		
		glfwSetKeyCallback(gl_window_, key_callback);
		glfwSetCursorPosCallback(gl_window_, mouse_callback);
		VK_CORE_INFO("Setup Window Callbacks!")
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

		if (key == GLFW_KEY_ESCAPE)
		{
			CloseHandle(gl_window);
		}
		
		window->key = key;
		window->scancode = scancode;
		window->action = action;
		window->mods = mods;
	}
	
	void Window::mouse_callback(GLFWwindow* gl_window, double xpos, double ypos)
	{
		//Gets the current window and assigns a new width and height
		auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(gl_window));
		window->xpos = xpos;
		window->ypos = ypos;
	}
}
