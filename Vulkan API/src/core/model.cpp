#include "model.h"

namespace Vulkan
{
	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> binding_descriptions(1 );
		binding_descriptions[0].binding = 0;
		binding_descriptions[0].stride = sizeof(Vertex);
		binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return binding_descriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2 );
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, position);
		
		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, colour);
		
		return attribute_descriptions;
	}

	Model::Model(Device& device, const std::vector<Vertex>& vertices) : device_(device)
	{
		create_vertex_buffers(vertices);
	}
	
	Model::~Model()
	{
		vkDestroyBuffer(device_.get_device(), vertex_buffer_, nullptr);
		vkFreeMemory(device_.get_device(), vertex_buffer_memory_, nullptr);
	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertex_buffer_ };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertex_count_, 1,  0,  0);
	}

	void Model::create_vertex_buffers(const std::vector<Vertex>& vertices)
	{
		vertex_count_ = static_cast<uint32_t>(vertices.size());
		VK_CORE_ASSERT(vertex_count_ >= 3, "Vertex count must be at least 3!");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertex_count_;

		device_.create_buffer(	bufferSize, 
								VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								vertex_buffer_,
								vertex_buffer_memory_);
		void* data;
		vkMapMemory(device_.get_device(), vertex_buffer_memory_, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device_.get_device(), vertex_buffer_memory_);
		
	}
}
