#include "App.h"

//-=-=-=-=- CORE -=-=-=-=-
#include "simpleRenderSystem.h"

//-=-=-=-=- GLFW -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//-=-=-=-=- STD -=-=-=-=-

namespace Vulkan
{
	
	App::App()
	{
		load_game_objects();
	}

	App::~App()
	{
	}

	void App::run()
	{
		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.get_swap_chain_render_pass() };

		//Keeps the window open until we need it to close
		while(!window_.should_close())
		{
			glfwPollEvents();
			VK_CORE_INFO("-=-=-=-=-=- NEW FRAME -=-=-=-=-=-")

			//Begins the rendering of the frame
			if (auto commandbuffer = renderer_.begin_frame())
			{
				renderer_.begin_swap_chain_render_pass(commandbuffer);
				simpleRenderSystem.render_game_objects(commandbuffer, game_objects_);
				renderer_.end_swap_chain_render_pass(commandbuffer);
				renderer_.end_frame();
			}
		}
		vkDeviceWaitIdle(device_.get_device());
	}

	void App::load_game_objects()
	{
		std::vector<Model::Vertex> vertices
		{

			{{ -0.5f,  0.5f },	{1.0f, 0.0f, 0.0f}},
			{{  0.5f,  0.5f },	{0.0f, 1.0f, 0.0f}},
			{{  0.0f, -0.5f },	{0.0f, 0.0f, 1.0f}}

		};

		auto model_ = std::make_shared<Model>(device_, vertices);
		auto triangle = GameObject::createGameObject();
		triangle.model = model_;
		triangle.colour = { .1f, .8f, .1f };
		triangle.transform_2d.translation.x = .2f;
		triangle.transform_2d.scale = { 2.f, .5f };
		triangle.transform_2d.rotation = .25f * glm::two_pi<float>();

		game_objects_.push_back(std::move(triangle));
	}
	
}
