#pragma once

#include "chunk.hpp"
#include "../core/camera.h"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace Vulkan
{
	class world
	{
	public:
		world();

		void add_chunk();
		void remove_chunk();
		
		void render(VkCommandBuffer commandBuffer, camera cam);
		void update();

	private:
		std::vector<Chunk> chunks_;
		std::vector<Block*> blocks_to_render_;

	};
}
