#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "pipeline.h"
#include "device.h"
#include "game_object.h"

//-=-=-=-=- STD -=-=-=-=-
#include <memory>
#include <vector>

namespace Vulkan
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(Device &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void render_game_objects(VkCommandBuffer commandbuffer, std::vector<GameObject> &gameObjects);
	
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
