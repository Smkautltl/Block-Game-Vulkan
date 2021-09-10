#include "world.h"
#include <future>

Vulkan::world::world(Device& device) : device_(device)
{	
	VK_CORE_WARN("Chunks are being generated...")
	auto bench = VK_CORE_BENCH("Chunk generation");
	//Generate block data for each of the chunks
	for (auto z = -ChunkZDistance; z < ChunkZDistance; z++)
	{
		for (auto x = -ChunkXDistance; x < ChunkXDistance; x++)
		{
			chunk_map_[x * unique_chunk_offset_ + z].update_chunk_data(x, z, generator_.getnoise(x, z));
		}
	}
	
	{//Culling the faces of the blocks within the the chunk
		std::vector<std::future<void>> culling;
		for (auto& chunk : chunk_map_)
		{
			culling.push_back(std::async([&](){ cull_chunk(chunk.second, chunk.second.x_, chunk.second.z_); }));
		}
	}
}
Vulkan::world::~world()
{
	VK_CORE_WARN("World destructor called!")
}

void Vulkan::world::add_chunk(int x, int z)
{
	chunk_map_[x * unique_chunk_offset_ + z].update_chunk_data(x, z, generator_.getnoise(x, z));
}
void Vulkan::world::cull_chunk(Chunk& chunk, int x, int z)
{
	Chunk* left = &BlankChunk;
	Chunk* right = &BlankChunk;
	Chunk* front = &BlankChunk;
	Chunk* back = &BlankChunk;

	if (chunk_map_.find((float)(x-1) * unique_chunk_offset_ + z) != chunk_map_.end())
	{
		left = &chunk_map_[(float)(x - 1) * unique_chunk_offset_ + (float)z];
	}
	if (chunk_map_.find((float)(x + 1) * unique_chunk_offset_ + z) != chunk_map_.end())
	{
		right = &chunk_map_[(float)(x + 1) * unique_chunk_offset_ + z];
	}
	if (chunk_map_.find((float)x * unique_chunk_offset_ + (float)(z - 1)) != chunk_map_.end())
	{
		front = &chunk_map_[(float)x * unique_chunk_offset_ + (float)(z - 1)];
	}
	if (chunk_map_.find((float)x * unique_chunk_offset_ + (float)(z + 1)) != chunk_map_.end())
	{
		back = &chunk_map_[(float)x * unique_chunk_offset_ + (float)(z + 1)];
	}
	
	chunk.load_block_faces(device_, left, right, front, back);
}
void Vulkan::world::remove_chunk(int x, int z)
{
	chunk_map_.erase((float)x * unique_chunk_offset_ + z);
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, Camera& cam, VkPipelineLayout& pipeline_layout_)
{
	auto projView = cam.get_proj_matrix() * cam.get_view_matrix();
	for (auto& chunk : chunk_map_)//TODO: Reduce number of draw calls (?)
	{
		if (chunk.second.ready)
		{
			
			std::lock_guard<std::mutex> lock(chunk.second.mutex);
			SimplePushConstantData push;
			push.position = chunk.second.transform_.translation;
			push.transform = projView * chunk.second.transform_.mat4();
			vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

			chunk.second.get()->bind(commandBuffer);
			chunk.second.get()->draw(commandBuffer);
		}
		chunk.second.being_used.notify_all();
	}	
}

void Vulkan::world::update(glm::vec3 CamPos)
{
	glm::vec3 cam_pos_chunk_coords = { (int)CamPos.x / 16, (int)CamPos.y / 16, (int)CamPos.z / 16 };	

	int camDeltax = (int)cam_pos_chunk_coords.x - (int)LastCamLocation.x;
	int camDeltaz = (int)cam_pos_chunk_coords.z - (int)LastCamLocation.z;
	
	if (camDeltax == 1 || camDeltax == -1)
	{
		int AddX = ((int)LastCamLocation.x) + (ChunkXDistance * camDeltax);
		int RemoveX = ((int)LastCamLocation.x) - (ChunkXDistance * camDeltax);

		//Create new chunks along the X axis
		for (auto i = -ChunkZDistance; i < ChunkZDistance; i++)
		{
			int Z = ((int)LastCamLocation.z) + i;
			add_chunk(AddX, Z);
			to_be_culled_.emplace_back(AddX, Z);

			if (chunk_map_.find((AddX + 1) * unique_chunk_offset_ + Z) != chunk_map_.end())
				to_be_culled_.emplace_back((AddX + 1), Z);
			
			if (chunk_map_.find((AddX - 1) * unique_chunk_offset_ + Z) != chunk_map_.end())
				to_be_culled_.emplace_back((AddX - 1), Z);
			
			if (chunk_map_.find(AddX * unique_chunk_offset_ + (Z + 1)) != chunk_map_.end())
				to_be_culled_.emplace_back(AddX, (Z + 1));
			
			if (chunk_map_.find(AddX * unique_chunk_offset_ + (Z - 1)) != chunk_map_.end())
				to_be_culled_.emplace_back(AddX, (Z - 1));

			to_be_deleted_.push_back(RemoveX * unique_chunk_offset_ + Z);
		}

		auto end = to_be_culled_.end();
		for (auto it = to_be_culled_.begin(); it != end; ++it) {
			end = std::remove(it + 1, end, *it);
		}
	}
	if (camDeltaz == 1 || camDeltaz == -1)
	{
		int AddZ = (int)LastCamLocation.z + (ChunkZDistance * camDeltaz);
		int RemoveZ = (int)LastCamLocation.z - (ChunkZDistance * camDeltaz);

		//Create new chunks along the Z axis
		for (auto i = -ChunkXDistance; i < ChunkXDistance; i++)
		{
			int X = (int)LastCamLocation.x + i;
			add_chunk(X, AddZ);
			to_be_culled_.emplace_back(X, AddZ);

			if (chunk_map_.find((X + 1) * unique_chunk_offset_ + AddZ) != chunk_map_.end())
			{
				to_be_culled_.emplace_back((X + 1), AddZ);			
			}
			if (chunk_map_.find((X - 1) * unique_chunk_offset_ + AddZ) != chunk_map_.end())
			{
				to_be_culled_.emplace_back((X - 1), AddZ);	
			}
			if (chunk_map_.find(X * unique_chunk_offset_ + (AddZ + 1)) != chunk_map_.end())
			{
				to_be_culled_.emplace_back(X, (AddZ + 1));	
			}
			if (chunk_map_.find(X * unique_chunk_offset_ + (AddZ - 1)) != chunk_map_.end())
			{
				to_be_culled_.emplace_back(X, (AddZ - 1));	
			}

			to_be_deleted_.push_back(X * unique_chunk_offset_ + RemoveZ);
		}
		
		auto end = to_be_culled_.end();
		for (auto it = to_be_culled_.begin(); it != end; ++it) {
			end = std::remove(it + 1, end, *it);
		}
	}

	if(!to_be_culled_.empty())
	{
		//Cull all the chunks that need to be culled
		auto bench = VK_CORE_BENCH("New chunks added in");
		
		std::vector<std::thread> CullChunksThreads;
		std::vector<glm::vec2> removefromtobeculled;
		int count = 0;
		for (auto chunk : to_be_culled_)
		{
			CullChunksThreads.emplace_back([=]() {

				std::lock_guard<std::mutex> lock(chunk_map_[chunk.x * unique_chunk_offset_ + chunk.y].mutex);
				cull_chunk(chunk_map_[chunk.x * unique_chunk_offset_ + chunk.y], chunk.x, chunk.y);
				});
			count++;
		}

		for (auto& cull_chunks_thread : CullChunksThreads)
		{
			cull_chunks_thread.join();
		}
		to_be_culled_.clear();
	}

	if(!to_be_deleted_.empty())
	{
		for (auto chunk : to_be_deleted_)
		{
			{
				std::unique_lock<std::mutex> lock(chunk_map_[chunk].mutex);
				chunk_map_[chunk].being_used.wait_for(lock, wait);
			}
			chunk_map_.erase(chunk);
		}
	}
	
	LastCamLocation = { cam_pos_chunk_coords.x, cam_pos_chunk_coords.y, cam_pos_chunk_coords.z};
}
