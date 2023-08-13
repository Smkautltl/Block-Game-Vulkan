#pragma once
#include <glm/glm.hpp>

#include "Device.h"

#define SHADOWMAP_CASCADE_COUNT 4
#define SHADOWMAP_SIZE 2048

struct Cascade
{
	VkFramebuffer FrameBuffer;
	VkDescriptorSet DescriptorSet;
	VkImageView View;

	float SplitDepth;
	glm::mat4 ViewProjMatrix;

	void destroy(VkDevice device) 
	{
		vkDestroyImageView(device, View, nullptr);
		vkDestroyFramebuffer(device, FrameBuffer, nullptr);
	}
};

class DepthTexture
{
public:
	DepthTexture(Device& device);
	~DepthTexture();

	static std::unique_ptr<DepthTexture> CreateDepthTexture(Device& device);
	VkRenderPass GetRenderPass() { return renderPass; }
	VkSampler GetSampler() { return depthSampler; }
	VkImageView GetView() { return depthImageView; }
	std::vector<Cascade>& GetCascades() { return cascades; }

private:
	void createDepthTexture();
	void createRenderPass();
	void createDepthTextureImageViews();
	void createDepthSampler();

public:

private:
	VkDescriptorImageInfo descriptor{};

	Device& device;
	VkRenderPass renderPass;

	VkImage depthImage = nullptr;
	VkDeviceMemory depthImageMemory = nullptr;
	VkImageView depthImageView = nullptr;
	VkSampler depthSampler = nullptr;

	std::vector<Cascade> cascades;

};