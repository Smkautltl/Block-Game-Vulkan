#include "world.h"

Vulkan::world::world(Device& device_)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds = end - start;
	uint32_t id = 0;
	
	for (auto z = 0; z < ChunkZDistance; z++)
	{
		std::vector<Chunk> chunkRow;
		for (auto x = 0; x < ChunkXDistance; x++)
		{			
			start = std::chrono::system_clock::now();
			
			Chunk chunk{id, x, z};
			chunk.generate();
			chunkRow.push_back(std::move(chunk));
			id++;
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;
			VK_CORE_INFO("Chunk: {0}-{1} created in {2}ms", z, x, std::to_string(elapsed_seconds.count()*1000))
		}
		chunks_.push_back(std::move(chunkRow));
	}
	
	int z = 0;	
	for (auto& chunkrow : chunks_)
	{
		int x = 0;
		for (auto& chunk : chunkrow)
		{
			start = std::chrono::system_clock::now();

			auto left = BlankChunk;
			auto right = BlankChunk;
			auto front = BlankChunk;
			auto back = BlankChunk;
			
			if (x - 1 >= 0)
				left = chunks_[z][x-1];
			if (x + 1 < ChunkXDistance)
				right = chunks_[z][x + 1];
			if (z - 1 >= 0)
				front = chunks_[z - 1][x];
			if (z + 1 < ChunkZDistance)
				back = chunks_[z + 1][x];
			
			chunk.load_block_faces(device_, left, right, front, back);
			
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;
			VK_CORE_INFO("Chunk: {0}-{1} faces culled in {2}ms", chunk.xz_Coords().first, chunk.xz_Coords().second, std::to_string(elapsed_seconds.count() * 1000))
			x++;
		}
		z++;
	}
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, camera& cam, VkPipelineLayout& pipeline_layout_)
{
	for (auto& chunkrow : chunks_)
	{
		for (auto& chunk : chunkrow)
		{
			SimplePushConstantData push;
			push.color = glm::vec3{ 0.5f, 0.5f, 0.5f };
			push.transform = cam.UpdateModelView(chunk.transform_.mat4());

			vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			chunk.get()->bind(commandBuffer);
			chunk.get()->draw(commandBuffer);
		}	
	}
}
