#pragma once

//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"

//-=-=-=-=- GLFW -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Vulkan
{
	class Model
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 colour;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};
		
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
		VkBuffer vertex_buffer_;
		VkDeviceMemory vertex_buffer_memory_;
		uint32_t vertex_count_;
	};
}
