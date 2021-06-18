#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"

namespace Vulkan
{
	
	class Model
	{
	public:
		
		Model(Device &device, const std::vector<Vertex>& vertices);
		~Model();

		//Deleting copy constructors
		Model(const Model&) = delete;
		Model &operator=(const Model&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void create_vertex_buffers(const std::vector<Vertex>& vertices);
	
	public:
	
	private:
		Device &device_;
		AllocatedBuffer vertex_buffer_;
		VkDeviceMemory vertex_buffer_memory_;
		uint32_t vertex_count_;
	};
}
