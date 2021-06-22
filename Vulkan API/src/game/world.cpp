#include "world.h"

Vulkan::world::world(Device& device) : device_(device)
{
	uint16_t id = 0;
	unsigned int blocktype = 0;

	for (auto z = -ChunkZDistance; z < ChunkZDistance; z++)
	{
		
		std::vector<Chunk> chunkRow;
		for (auto x = -ChunkXDistance; x < ChunkXDistance; x++)
		{			
			VK_CORE_BENCH bench{ "Generation - Chunk ID:" + std::to_string(id) };
			Chunk chunk{id, x, z};
			chunk.generate();
			chunkRow.push_back(std::move(chunk));
			id++;
			
		}
		chunks_.push_back(std::move(chunkRow));
	}
	
	int z = 0;	
	for (auto& chunkrow : chunks_)
	{
		int x = 0;
		for (auto& chunk : chunkrow)
		{
			VK_CORE_BENCH bench{ "Face Culling & Model Creation - Chunk ID:" + std::to_string(id) };
			
			Chunk* left = &BlankChunk;
			Chunk* right = &BlankChunk;
			Chunk* front = &BlankChunk;
			Chunk* back = &BlankChunk;

			if (x - 1 >= 0)
				left = &chunks_[z][x - 1];
				
			if (x + 1 < ChunkXDistance*2)
				right = &chunks_[z][x + 1];
				
			if (z - 1 >= 0)
				front = &chunks_[z - 1][x];
				
			if (z + 1 < ChunkZDistance*2)
				back = &chunks_[z + 1][x];
				
			chunk.load_block_faces(device_, left, right, front, back);
			x++;
		}		
		z++;
	}
}

Vulkan::world::~world()
{
	VK_CORE_WARN("World destructor called!")
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_)
{
	for (auto& chunkrow : chunks_)
	{
		for (auto& chunk : chunkrow)
		{
			if (chunk.is_model_valid())
			{
				SimplePushConstantData push;
				push.color = glm::vec3{ 0.5f, 0.5f, 0.5f };
				push.transform = cam.get_proj_matrix() * cam.get_view_matrix() * chunk.transform_.mat4();

				vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
				chunk.get()->bind(commandBuffer);
				chunk.get()->draw(commandBuffer);
			}
		}	
	}
}
