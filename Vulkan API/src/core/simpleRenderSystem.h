#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "pipeline.h"
#include "device.h"
//#include "game_object.h"

//-=-=-=-=- STD -=-=-=-=-
#include <memory>
//#include <vector>

#include "camera.h"
#include "../game/world.h"

namespace Vulkan
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(Device &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void render_game_objects(VkCommandBuffer commandbuffer, camera& cam);
	
	private:
		void create_pipeline_layout();
		void create_pipeline(VkRenderPass renderPass);	

	public:


	private:
		Device& device_;
		std::unique_ptr<pipeline> pipeline_;
		VkPipelineLayout pipeline_layout_;

		world* world_;
	};
}
