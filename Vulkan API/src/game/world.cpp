#include "world.h"

#include <future>

Vulkan::world::world(Device& device) : device_(device)
{	
	unsigned int blocktype = 0;
	
	for (auto z = -ChunkZDistance; z < ChunkZDistance; z++)
	{
		for (auto x = -ChunkXDistance; x < ChunkXDistance; x++)
		{
			auto bench = VK_CORE_BENCH{ "Chunk Generated" };
			ChunkMap[(x * 1.005f) + z].update_chunkdata(x,z);
			ChunkMap[(x * 1.005f) + z].generate(generator_.getnoise(x,z));

		}
	}
	
	int z = 0;	
	for (auto& chunk : ChunkMap)
	{		
		cull_chunk(chunk.second, chunk.second.x_, chunk.second.z_);
	}

}

Vulkan::world::~world()
{
	VK_CORE_WARN("World destructor called!")
}

void Vulkan::world::add_chunk(int x, int z)
{
	Chunk newChunk{ x,z };
	newChunk.generate(generator_.getnoise(x, z));
	cull_chunk(newChunk, x, z);

	if (ChunkMap.find((float)(x - 1) * 1.005f + z) != ChunkMap.end())
	{
		cull_chunk(ChunkMap[(float)(x - 1) * 1.005f + (float)z],x - 1 , z);
	}
	if (ChunkMap.find((float)(x + 1) * 1.005f + z) != ChunkMap.end())
	{
		cull_chunk(ChunkMap[(float)(x + 1) * 1.005f + (float)z], x + 1, z);
	}
	if (ChunkMap.find((float)x * 1.005f + (float)(z - 1)) != ChunkMap.end())
	{
		cull_chunk(ChunkMap[(float)x * 1.005f + (float)(z - 1)], x, z - 1);
	}
	if (ChunkMap.find((float)x * 1.005f + (float)(z + 1)) != ChunkMap.end())
	{
		cull_chunk(ChunkMap[(float)x * 1.005f + (float)(z + 1)], x, z + 1);
	}

}

void Vulkan::world::cull_chunk(Chunk& chunk, int x, int z)
{
	auto bench = VK_CORE_BENCH("Chunk created");
	Chunk* left = &BlankChunk;
	Chunk* right = &BlankChunk;
	Chunk* front = &BlankChunk;
	Chunk* back = &BlankChunk;

	if (ChunkMap.find((float)(x-1) * 1.005f + z) != ChunkMap.end())
	{
		left = &ChunkMap[(float)(x - 1) * 1.005f + (float)z];
	}
	if (ChunkMap.find((float)(x + 1) * 1.005f + z) != ChunkMap.end())
	{
		right = &ChunkMap[(float)(x + 1) * 1.005f + z];
	}
	if (ChunkMap.find((float)x * 1.005f + (float)(z - 1)) != ChunkMap.end())
	{
		front = &ChunkMap[(float)x * 1.005f + (float)(z - 1)];
	}
	if (ChunkMap.find((float)x * 1.005f + (float)(z + 1)) != ChunkMap.end())
	{
		back = &ChunkMap[(float)x * 1.005f + (float)(z + 1)];
	}
	
	chunk.load_block_faces(device_, left, right, front, back);
}

void Vulkan::world::remove_chunk(int x, int z)
{
	ChunkMap.erase((float)x * 1.005f + z);
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_)
{
	auto updateLamda = [&]() {update(cam.get_camera_pos());};
	auto asyncUpdate = std::async(updateLamda);
	
	auto projView = cam.get_proj_matrix() * cam.get_view_matrix();
	
	for (auto& chunk : ChunkMap)
	{
			if (chunk.second.is_model_valid())
			{
				SimplePushConstantData push;
				push.color = glm::vec3{ 0.5f, 0.5f, 0.5f };
				push.position = chunk.second.transform_.translation;
				push.transform = projView * chunk.second.transform_.mat4();

				vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
				chunk.second.get()->bind(commandBuffer);
				chunk.second.get()->draw(commandBuffer);
			}
			
	}

	asyncUpdate.wait();
}

void Vulkan::world::update(glm::vec3 CamPos)
{
	VK_CORE_TRACE("Location - X:{0},Y:{1},Z:{2}", (int)CamPos.x/16, (int)CamPos.y, (int)CamPos.z/16)

	
	
}
