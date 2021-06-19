#include "app.h"

//-=-=-=-=- CORE -=-=-=-=-
#include "simpleRenderSystem.h"

//-=-=-=-=- GLFW -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../game/world.h"

//-=-=-=-=- STD -=-=-=-=-

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
		
		//Keeps the window open until we need it to close
		while (!window_.should_close())
		{
			glfwPollEvents();
			VK_CORE_INFO("-=-=-=-=-=- NEW FRAME -=-=-=-=-=-")

				cam.update_camera_pos(window_.key, window_.scancode, window_.action, window_.mods);
			cam.update_camera_rot(window_.xpos, window_.ypos);

			//window_.recentre_mouse();
			cam.recalculate_proj_matrix(70.f, window_.get_extent().width, window_.get_extent().height);

			//Begins the rendering of the frame
			if (auto commandbuffer = renderer_.begin_frame())
			{
				renderer_.begin_swap_chain_render_pass(commandbuffer);

				simpleRenderSystem.render_game_objects(commandbuffer, cam);

				renderer_.end_swap_chain_render_pass(commandbuffer);
				renderer_.end_frame();
			}
		}
		
		vkDeviceWaitIdle(device_.get_device());		
	}

	std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
		std::vector<Vertex> vertices
		{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices)
		{
			v.position += offset;
		}
		return std::make_unique<Model>(186367385, device, vertices);
	}
	
	void App::load_game_objects()
	{
		//Block blocktype1{device_, {0.5f, 0.1f, 0.1f}};
		//Block blocktype2{device_, {0.2f, 0.2f, 0.2f}};
		//Block blocktype3{device_, {0.1f, 0.1f, 0.5f}};
		//
		//std::vector<Block> blockTypes;
		//blockTypes.push_back(std::move(blocktype1));
		//blockTypes.push_back(std::move(blocktype2));
		//blockTypes.push_back(std::move(blocktype3));
		//
		//for (auto i = 0; i < 1; i++)
		//{
		//	Chunk chnk;
		//	chnk.load_game_objects(device_, blockTypes, game_objects_);
		//	chunks.push_back(std::move(chnk));
		//}

		//std::shared_ptr<Model> model = createCubeModel(device_, { 0,0,0 });
		//
		//for (auto i = 0; i < 100; i++)
		//{
		//
		//	
		//	auto cube = GameObject::createGameObject();
		//	cube.model = model;
		//	cube.transform_.translation = { (rand() % 100)-50, (rand() % 100) - 50, (rand() % 100) - 50 };
		//	cube.transform_.scale = { (rand() % 5) + 0.1, (rand() % 5) + 0.1, (rand() % 5) + 0.1 };
		//	game_objects_.push_back(std::move(cube));
		//}	
	}
}
