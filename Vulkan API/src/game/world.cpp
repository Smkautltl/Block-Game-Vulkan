#include "world.h"

#include <future>

Vulkan::world::world(Device& device) : device_(device)
{
	VK_CORE_WARN("Chunks are being generated...")
	
	{//Generate block data for each of the chunks
		std::vector<std::future<void>> generate;
		for (auto z = -ChunkZDistance; z < ChunkZDistance; z++)
		{
			for (auto x = -ChunkXDistance; x < ChunkXDistance; x++)
			{
				chunk_map_[x * 1.005f + z].update_chunkdata(x, z, generator_.getnoise(x, z));
			}
		}
	}

	{//Culling the faces of the blocks within the the chunk
		std::vector<std::future<void>> culling;
		for (auto& chunk : chunk_map_)
		{
			culling.push_back(std::async([&]()
			{
				cull_chunk(chunk.second, chunk.second.x_, chunk.second.z_);
			}));
		}
	}
	VK_CORE_WARN("Complete!")
}

Vulkan::world::~world()
{
	VK_CORE_WARN("World destructor called!")
}

void Vulkan::world::add_chunk(int x, int z)
{
	chunk_map_[x * 1.005f + z].update_chunkdata(x, z, generator_.getnoise(x, z));
}

void Vulkan::world::cull_chunk(Chunk& chunk, int x, int z)
{
	//std::string name = "Chunk X:" + std::to_string(x) + " == Z:" + std::to_string(z) + "\tculling";
	//auto bench = VK_CORE_BENCH{ name };
	Chunk* left = &BlankChunk;
	Chunk* right = &BlankChunk;
	Chunk* front = &BlankChunk;
	Chunk* back = &BlankChunk;

	if (chunk_map_.find((float)(x-1) * 1.005f + z) != chunk_map_.end())
	{
		left = &chunk_map_[(float)(x - 1) * 1.005f + (float)z];
	}
	if (chunk_map_.find((float)(x + 1) * 1.005f + z) != chunk_map_.end())
	{
		right = &chunk_map_[(float)(x + 1) * 1.005f + z];
	}
	if (chunk_map_.find((float)x * 1.005f + (float)(z - 1)) != chunk_map_.end())
	{
		front = &chunk_map_[(float)x * 1.005f + (float)(z - 1)];
	}
	if (chunk_map_.find((float)x * 1.005f + (float)(z + 1)) != chunk_map_.end())
	{
		back = &chunk_map_[(float)x * 1.005f + (float)(z + 1)];
	}
	
	chunk.load_block_faces(device_, left, right, front, back);
}

void Vulkan::world::remove_chunk(int x, int z)
{
	chunk_map_.erase((float)x * 1.005f + z);
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_)
{
	//TODO: Stop stuttering on chunk creation and deletion && fix crashing
	
	auto projView = cam.get_proj_matrix() * cam.get_view_matrix();
	for (auto& chunk : chunk_map_)//TODO: Reduce number of draw calls (?)
	{
		if (chunk.second.ready && chunk.second.lock.try_lock())
		{
			//chunk.second.lock.lock();
			SimplePushConstantData push;
			push.position = chunk.second.transform_.translation;
			push.transform = projView * chunk.second.transform_.mat4();
			//push.color = glm::vec3{ 0.5f, 0.5f, 0.5f };
			vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

			chunk.second.get()->bind(commandBuffer);
			chunk.second.get()->draw(commandBuffer);
			chunk.second.lock.unlock();		
		}	
	}

	/*
	for (auto chunk : to_be_deleted_)
	{
		chunk_map_.erase(chunk);
	}
	to_be_deleted_.clear();
	
	for (auto chunk_to_cull : to_be_culled_)
	{
		std::vector<std::future<void>> cullingAsync;
		
		cullingAsync.push_back(std::async([&]() {cull_chunk(chunk_map_[chunk_to_cull.x * 1.005f + chunk_to_cull.x], chunk_to_cull.x, chunk_to_cull.y); } ));

		cullingAsync.push_back(std::async([&]()
			{
				if (chunk_map_.find((chunk_to_cull.x - 1) * 1.005f + chunk_to_cull.y) != chunk_map_.end())
				{
					auto& chunk = chunk_map_[(chunk_to_cull.x - 1) * 1.005f + chunk_to_cull.y];
					chunk.lock.try_lock();
					cull_chunk(chunk, chunk.x_, chunk.z_);
				}
			}));
		cullingAsync.push_back(std::async([&]()
		{
				if (chunk_map_.find((chunk_to_cull.x + 1) * 1.005f + chunk_to_cull.y) != chunk_map_.end())
				{
					auto& chunk = chunk_map_[(chunk_to_cull.x + 1) * 1.005f + chunk_to_cull.y];
					chunk.lock.try_lock();
					cull_chunk(chunk, chunk.x_, chunk.z_);
				}
		}));
		cullingAsync.push_back(std::async([&]()
		{
				if (chunk_map_.find(chunk_to_cull.x * 1.005f + (chunk_to_cull.y - 1)) != chunk_map_.end())
				{
					auto& chunk = chunk_map_[chunk_to_cull.x * 1.005f + (chunk_to_cull.y - 1)];
					chunk.lock.try_lock();
					cull_chunk(chunk, chunk.x_, chunk.z_);
				}
		}));
		cullingAsync.push_back(std::async([&]()
		{
				if (chunk_map_.find(chunk_to_cull.x * 1.005f + (chunk_to_cull.y + 1)) != chunk_map_.end())
				{
					auto& chunk = chunk_map_[chunk_to_cull.x * 1.005f + (chunk_to_cull.y + 1)];
					if (chunk.lock.try_lock())
					{
						cull_chunk(chunk, chunk.x_, chunk.z_);
					}
				}
		}));
	
	}
	to_be_culled_.clear();
	//update_async.wait();
	*/
}

void Vulkan::world::update(glm::vec3 CamPos)
{
	glm::vec3 cam_pos_chunk_coords = { (int)CamPos.x / 16, (int)CamPos.y / 16, (int)CamPos.z / 16 };	

	int camDeltax = (int)cam_pos_chunk_coords.x - (int)LastCamLocation.x;
	//int camDeltay = (int)cam_pos_chunk_coords.y - (int)LastCamLocation.y;
	int camDeltaz = (int)cam_pos_chunk_coords.z - (int)LastCamLocation.z;

	/*if (camDeltax == 1 || camDeltax == -1)
	{
		int AddX = ((int)LastCamLocation.x) + (ChunkXDistance * camDeltax);
		int RemoveX = ((int)LastCamLocation.x) - (ChunkXDistance * camDeltax);
		
		for (auto i = -ChunkZDistance; i < ChunkZDistance; i++)
		{
			int Z = ((int)LastCamLocation.z) + i;

			add_chunk(AddX, Z);
			to_be_culled_.push_back({AddX, Z});
			VK_CORE_WARN("Added chunk at X:{0}-Z:{1}", AddX, Z)
			
			to_be_deleted_.push_back(RemoveX * 1.005f + Z);
			VK_CORE_WARN("Chunk at X:{0}-Z:{1} set to be removed", RemoveX, Z)
		}	
	}
	//if (camDeltaz == 1 || camDeltaz == -1)
	{
		int AddZ = (int)LastCamLocation.z + (ChunkZDistance * camDeltaz);
		int RemoveZ = (int)LastCamLocation.z - (ChunkZDistance * camDeltaz);
		
		for (auto i = -ChunkXDistance; i < ChunkXDistance; i++)
		{
			int X = (int)LastCamLocation.x + i;
			
			add_chunk(X, AddZ);
			to_be_culled_.push_back({ X, AddZ });
			VK_CORE_WARN("Added chunk at X:{0}-Z:{1}", X, AddZ)
			
			to_be_deleted_.push_back(X * 1.005f + RemoveZ);
			VK_CORE_WARN("Chunk at X:{0}-Z:{1} set to be removed", X, RemoveZ)
		}
	}
	*/
	
	LastCamLocation = { cam_pos_chunk_coords.x, cam_pos_chunk_coords.y, cam_pos_chunk_coords.z};
}
