#include "Model.h"
#include "Core/Vulkan/Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
	template<>
	struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			size_t seed = 0;
			hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

Model::Model(Device& Device, const Builder& builder) : device(Device)
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}
Model::~Model()
{
}

std::unique_ptr<Model> Model::CreateFromFile(Device& device, const std::string& filepath)
{
	Builder builder{};
	builder.loadModel(filepath);
	//VK_CORE_INFO("Vertex Count: {0}", builder.vertices.size())

	return std::make_unique<Model>(device, builder);
}

//Public
void Model::Bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
	VkDeviceSize offset[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	if (hasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}
void Model::Draw(VkCommandBuffer commandBuffer)
{
	if (hasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		return;
	}
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

//Private
void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
{
	vertexCount = static_cast<uint32_t>(vertices.size());
	VK_CORE_ASSERT(vertexCount >= 3, "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
	uint32_t vertexSize = sizeof(vertices[0]);

	Buffer stagingBuffer{device, vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)vertices.data());

	vertexBuffer = std::make_unique<Buffer>(device, vertexSize, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

}
void Model::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());
	hasIndexBuffer = indexCount > 0;

	if (!hasIndexBuffer)
	{
		return;
	}

	VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
	uint32_t indexSize = sizeof(indices[0]);

	Buffer stagingBuffer{ device, indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)indices.data());

	indexBuffer = std::make_unique<Buffer>(device, indexSize, indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	device.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

}

void Model::Builder::loadModel(const std::string& filepath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};
			if (index.vertex_index >= 0)
			{
				vertex.pos = { attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2] };

			}

			vertex.color = { attrib.colors[index.vertex_index + 0], attrib.colors[index.vertex_index + 1], attrib.colors[index.vertex_index + 2] };

			if (index.normal_index >= 0)
			{
				vertex.normal = { attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2] };

			}
			if (index.texcoord_index >= 0)
			{
				vertex.uv = { attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1] };

			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
				//VK_CORE_TRACE("\tVERTEX: (({:03.4f}, {:03.4f}, {:03.4f}), ({:03.4f}, {:03.4f}, {:03.4f}), ({:03.4f}, {:03.4f}, {:03.4f}), ({:03.4f}, {:03.4f}))", vertex.pos.x, vertex.pos.y, vertex.pos.z, vertex.color.x, vertex.color.y, vertex.color.z, vertex.normal.x, vertex.normal.y, vertex.normal.z, vertex.uv.x, vertex.uv.y);
				//VK_CORE_TRACE("")
			}
			indices.push_back(uniqueVertices[vertex]);
			//VK_CORE_TRACE("\tINDEX: {0}", uniqueVertices[vertex]);
		}
	}
}