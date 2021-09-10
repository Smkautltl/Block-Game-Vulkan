#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

Vulkan::Texture::~Texture()
{
	device_.destroy_buffer(stagingBuffer);
	vkDestroyImageView(device_.get_device(), image_view_, nullptr);
	device_.destroy_image(newImage);
	vkDestroySampler(device_.get_device(), textureSampler, nullptr);
}

bool Vulkan::Texture::load_image_from_file(const char* filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	
	if(!pixels)
	{
		VK_CORE_CRITICAL("Failed to load Texture file - {0}", filename)
		return false;
	}

	VkDeviceSize imageSize = (texWidth * texHeight * 4);

	stagingBuffer = device_.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, imageSize, pixels);
	stbi_image_free(pixels);

	VkImageCreateInfo dImgInfo;
		dImgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

		dImgInfo.imageType = VK_IMAGE_TYPE_2D;

		dImgInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		dImgInfo.extent.width = texWidth;
		dImgInfo.extent.height = texHeight;
		dImgInfo.extent.depth = 1;

		dImgInfo.mipLevels = 1;
		dImgInfo.arrayLayers = 1;
		dImgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		dImgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		dImgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		dImgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	device_.create_image(dImgInfo, VMA_MEMORY_USAGE_GPU_ONLY, newImage);
	device_.transition_image_layout(newImage._image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	device_.copy_buffer_to_image(stagingBuffer.buffer_, newImage._image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	swap_chain_.create_texture_image_view(image_view_, newImage._image);
	
	return true;
}

void Vulkan::Texture::create_texture_sampler()
{
	VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = device_.properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if(vkCreateSampler(device_.get_device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to create Texture sampler!")
		}
}