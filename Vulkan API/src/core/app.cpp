#include "app.h"

//-=-=-=-=- CORE -=-=-=-=-
#include "simpleRenderSystem.h"

//-=-=-=-=- GLFW -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//-=-=-=-=- GAME -=-=-=-=-
#include "../game/world.h"

//-=-=-=-=- STD -=-=-=-=-
#include <thread>

namespace Vulkan
{

	App::App()
	{
		//load_game_objects();
	}

	App::~App()
	{
		VK_CORE_WARN("App destructor called!");
		game_objects_.clear();
	}

	void App::run()
	{
		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.get_swap_chain_render_pass() };
		auto func = [this] { realtime_functions(); };
		std::thread realTimeThread (func);
		
		//Keeps the window open until we need it to close
		while (!window_.should_close())
		{
			glfwPollEvents();
			VK_CORE_INFO("-=-=-=-=-=- NEW FRAME -=-=-=-=-=-")

			//Begins the rendering of the frame
			if (auto commandbuffer = renderer_.begin_frame())
			{
				renderer_.begin_swap_chain_render_pass(commandbuffer);

				simpleRenderSystem.render_game_objects(commandbuffer, cam);

				renderer_.end_swap_chain_render_pass(commandbuffer);
				renderer_.end_frame();
			}
		}

		realTimeThread.join();
		vkDeviceWaitIdle(device_.get_device());		
	}


	
	void App::load_game_objects()
	{
		
	}

	void App::realtime_functions()
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> lasttime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_ms = std::chrono::high_resolution_clock::now() - lasttime;
		
		while (!window_.should_close())
		{
			elapsed_ms = std::chrono::high_resolution_clock::now() - lasttime;
			if ((elapsed_ms.count() * 1000) >= 1.0)
			{
				glfwPollEvents();
				cam.update_camera_pos(window_.key, window_.scancode, window_.action, window_.mods);
				cam.update_camera_rot(window_.xpos, window_.ypos);
				cam.recalculate_proj_matrix(70.f, window_.get_extent().width, window_.get_extent().height);
				lasttime = std::chrono::high_resolution_clock::now();
			}	
		}
	}
}
