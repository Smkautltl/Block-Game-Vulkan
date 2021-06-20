#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"

namespace Vulkan
{
	
	class Model
	{
	public:
		
		Model(uint32_t id_, Device &device, const std::vector<Vertex>& vertices);
		~Model();

		//Deleting copy constructors
		Model(const Model&) = delete;
		Model &operator=(const Model&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		void destroy();

	private:
		void create_vertex_buffers(const std::vector<Vertex>& vertices);
	
	public:
	
	private:
		uint32_t id_;
		Device &device_;
		AllocatedBuffer vertex_buffer_;
		uint32_t vertex_count_;
	};
}
