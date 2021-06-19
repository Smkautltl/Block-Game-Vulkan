#pragma once

#include "chunk.h"
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
		std::vector<std::vector<Chunk>> chunks_;
		Chunk BlankChunk{ UINT32_MAX, 0,0 };
		int ChunkXDistance = 5;
		int ChunkZDistance = 1;
	};
}
