#pragma once
//-=-=-=-=- STD -=-=-=-=-
#include <vector>

//-=-=-=-=- VULKAN -=-=-=-=-
#include <vulkan/vulkan_core.h>

//-=-=-=-=- CORE -=-=-=-=-
#include "../core/camera.h"

//-=-=-=-=- GAME -=-=-=-=-
#include <map>

#include "chunk.h"
#include "TerrainGeneration/TerrainGenerator.h"

namespace Vulkan
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.0f };
		glm::vec3 position;
		alignas(16) glm::vec3 color;
	};
	
	class world
	{
	public:
		world(Device& device_);
		~world();

		void add_chunk(int x, int z);
		void cull_chunk(Chunk& chunk, int x, int z);
		void remove_chunk(int x, int z);

		void render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_);
		void update(glm::vec3 CamPos);

	private:
		Device& device_;

		int ChunkXDistance = 24;
		int ChunkZDistance = 24;
		TerrainGenerator generator_{8};
		
		std::map<float, Chunk> ChunkMap;	
		Chunk BlankChunk{0,0};

		glm::vec3 LastCamLocation;
	};
}
