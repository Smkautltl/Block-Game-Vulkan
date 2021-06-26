#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "pipeline.h"
#include "device.h"
#include "camera.h"

//-=-=-=-=- STD -=-=-=-=-
#include <memory>

//-=-=-=-=- GAME -=-=-=-=-
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

		void render_game_objects(VkCommandBuffer commandbuffer, Camera& cam, world& world);
	
	private:
		void create_pipeline_layout();
		void create_pipeline(VkRenderPass renderPass);	

	public:
	
	private:
		Device& device_;
		std::unique_ptr<pipeline> pipeline_;
		VkPipelineLayout pipeline_layout_;
	
	};
}
