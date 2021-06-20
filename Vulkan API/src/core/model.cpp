#include "model.h"

namespace Vulkan
{
	std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> binding_descriptions(1 );
		binding_descriptions[0].binding = 0;
		binding_descriptions[0].stride = sizeof(Vertex);
		binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return binding_descriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
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

	Model::Model(uint32_t id, Device& device, const std::vector<Vertex>& vertices) : id_(id), device_(device)
	{
		VK_CORE_WARN("Model ID:{0} | Created!", id)
		create_vertex_buffers(vertices);
	}
	
	Model::~Model()
	{
		
	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertex_buffer_.buffer_ };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertex_count_, 1,  0,  0);
	}

	void Model::destroy()
	{
		VK_CORE_WARN("Model ID:{0} | Destructor called!", id_)
		if (vertex_buffer_.buffer_ != nullptr)
		{
			device_.destroy_buffer(vertex_buffer_);
		}
		
	}

	void Model::create_vertex_buffers(const std::vector<Vertex>& vertices)
	{
		
		vertex_count_ = static_cast<uint32_t>(vertices.size());
		VK_CORE_ASSERT(vertex_count_ >= 3, "Vertex count must be at least 3!")

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertex_count_;

		device_.create_buffer(	bufferSize, 
								VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
								vertex_buffer_,
								vertices);
		VK_CORE_WARN("Model ID:{0} | Vertex Buffer Created!", id_)
	}
}
