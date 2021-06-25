#pragma once
//-=-=-=-=- STD -=-=-=-=-
#include <vector>

//-=-=-=-=- VULKAN -=-=-=-=-
#include <vulkan/vulkan_core.h>

//-=-=-=-=- CORE -=-=-=-=-
#include "../core/camera.h"

//-=-=-=-=- GAME -=-=-=-=-
#include "chunk.h"
#include "TerrainGeneration/TerrainGenerator.h"

namespace Vulkan
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.0f };
		glm::mat3 normal;
		alignas(16) glm::vec3 color;
	};
	
	class world
	{
	public:
		world(Device& device_);
		~world();

		//void add_chunk();
		void cull_chunk(Chunk& chunk, int x, int z);
		//void remove_chunk(int id);

		void render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_);
		//void update();

	private:
		Device& device_;

		int ChunkXDistance = 24;
		int ChunkZDistance = 24;
		TerrainGenerator generator_{8};
		
		typedef std::vector<Chunk> ChunkRow;
		typedef std::vector<ChunkRow> ChunkPlane;
		ChunkPlane chunks_;
		Chunk BlankChunk{ UINT32_MAX, 0,0};

		
	};
}
