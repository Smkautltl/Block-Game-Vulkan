#pragma once

#include "chunk.hpp"
#include "../core/camera.h"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace Vulkan
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.0f };
		alignas(16) glm::vec3 color;
	};
	
	class world
	{
	public:
		world(Device& device_);

		void add_chunk();
		void remove_chunk();
		
		void render(VkCommandBuffer commandBuffer, camera& cam, VkPipelineLayout& pipeline_layout_);
		void update();

	private:
		std::vector<Chunk> chunks_;

	};
}
