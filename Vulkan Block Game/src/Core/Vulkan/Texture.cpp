#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cmath>
#include <stdexcept>

#include "Texture.h"

Texture::Texture(Device& device, const std::string& textureFilepath) : device{ device }
{
    createTextureImage(textureFilepath);
    createTextureImageView(VK_IMAGE_VIEW_TYPE_2D);
    createTextureSampler();
    UpdateDescriptor();
}
Texture::Texture(Device& device, const std::string(&filepaths)[6]) : device{ device }
{
    createCubeMap(filepaths);
    createTextureImageView(VK_IMAGE_VIEW_TYPE_CUBE);
    createTextureSampler();
    UpdateDescriptor();
}
Texture::Texture(Device& device,VkFormat format,VkExtent3D extent,VkImageUsageFlags usage,VkSampleCountFlagBits sampleCount) : device{ device } 
{
    VkImageAspectFlags aspectMask = 0;
    VkImageLayout imageLayout;

    format = format;
    extent = extent;

    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = sampleCount;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = usage;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    device.CreateImageWithInfo( imageInfo,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,textureImage,textureImageMemory);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange = {};
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.image = textureImage;
    if (vkCreateImageView(device.vkDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) 
    {
        VK_CORE_RUNTIME("failed to create texture image view!");
    }

    // Sampler should be seperated out
    if (usage & VK_IMAGE_USAGE_SAMPLED_BIT) 
    {
        // Create sampler to sample from the attachment in the fragment shader
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = samplerInfo.addressModeU;
        samplerInfo.addressModeW = samplerInfo.addressModeU;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

        if (vkCreateSampler(device.vkDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) 
        {
            VK_CORE_RUNTIME("failed to create sampler!");
        }

        VkImageLayout samplerImageLayout = imageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        descriptor.sampler = textureSampler;
        descriptor.imageView = textureImageView;
        descriptor.imageLayout = samplerImageLayout;
    }
}
Texture::~Texture() 
{
    vkDestroySampler(device.vkDevice(), textureSampler, nullptr);
    vkDestroyImageView(device.vkDevice(), textureImageView, nullptr);
    vkDestroyImage(device.vkDevice(), textureImage, nullptr);
    vkFreeMemory(device.vkDevice(), textureImageMemory, nullptr);
}

std::unique_ptr<Texture> Texture::CreateTextureFromFile(Device& device, const std::string& filepath) 
{
    return std::make_unique<Texture>(device, filepath);
}
std::unique_ptr<Texture> Texture::CreateCubeMap(Device& device, const std::string(&filepaths)[6])
{
    return std::make_unique<Texture>(device, filepaths);
}

void Texture::UpdateDescriptor() 
{
    descriptor.sampler = textureSampler;
    descriptor.imageView = textureImageView;
    descriptor.imageLayout = textureLayout;
}

void Texture::createTextureImage(const std::string& filepath) 
{
    int texWidth, texHeight, texChannels;
    stbi_set_flip_vertically_on_load(1);  // todo determine why texture coordinates are flipped
    stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) 
    {
        VK_CORE_RUNTIME("failed to load texture image!");
    }

    mipLevels = 1;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    device.CreateBuffer(imageSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.vkDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device.vkDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    format = VK_FORMAT_R8G8B8A8_SRGB;
    extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = layerCount;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    device.TransitionImageLayout( textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, layerCount);
    device.CopyBufferToImage( stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), layerCount);

    // comment this out if using mips
    device.TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, layerCount);

    // If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
    // device.generateMipmaps(textureImage, format, texWidth, texHeight, mipLevels);
    textureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkDestroyBuffer(device.vkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.vkDevice(), stagingBufferMemory, nullptr);
}
void Texture::createCubeMap(const std::string(&filepaths)[6])
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels[6];

    // stbi_set_flip_vertically_on_load(1);  // todo determine why texture coordinates are flipped
    pixels[0] = stbi_load(filepaths[0].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[1] = stbi_load(filepaths[1].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[2] = stbi_load(filepaths[2].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[3] = stbi_load(filepaths[3].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[4] = stbi_load(filepaths[4].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[5] = stbi_load(filepaths[5].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
    VkDeviceSize layerSize = imageSize / 6;

    if (!pixels)
    {
        VK_CORE_RUNTIME("failed to load texture image!");
    }

    mipLevels = 1;
    layerCount = 6;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    uint32_t bytes_copied = 0;

    vkMapMemory(device.vkDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    for (uint8_t i = 0; i < 6; i++)
    {
        memcpy(data, pixels[i], static_cast<size_t>(layerSize));
    }
    vkUnmapMemory(device.vkDevice(), stagingBufferMemory);

    stbi_image_free(pixels[0]);
    stbi_image_free(pixels[1]);
    stbi_image_free(pixels[2]);
    stbi_image_free(pixels[3]);
    stbi_image_free(pixels[4]);
    stbi_image_free(pixels[5]);

    format = VK_FORMAT_R8G8B8A8_SRGB;
    extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = layerCount;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    device.TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, layerCount);
    device.CopyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), layerCount);

    // comment this out if using mips
    device.TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, layerCount);

    // If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
    // device.generateMipmaps(textureImage, format, texWidth, texHeight, mipLevels);
    textureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkDestroyBuffer(device.vkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.vkDevice(), stagingBufferMemory, nullptr);
}

void Texture::createTextureImageView(VkImageViewType viewType) 
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = viewType;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    if (vkCreateImageView(device.vkDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) 
    {
        VK_CORE_RUNTIME("failed to create texture image view!");
    }
}
void Texture::createTextureSampler() 
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    // these fields useful for percentage close filtering for shadow maps
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);

    if (vkCreateSampler(device.vkDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) 
    {
        VK_CORE_RUNTIME("failed to create texture sampler!");
    }
}

void Texture::TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout) 
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = textureImage;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) 
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else 
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&  newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
    {
        // This says that any cmd that acts in color output or after (dstStage)
        // that needs read or write access to a resource
        // must wait until all previous read accesses in fragment shader
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else 
    {
        VK_CORE_CRITICAL("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

//*********** SAMPLER ***********

Sampler::Sampler(Device& device) : device(device)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    // these fields useful for percentage close filtering for shadow maps
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device.vkDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to create texture sampler!");
    }
}
Sampler::~Sampler()
{
    vkDestroySampler(device.vkDevice(), sampler, nullptr);
}

std::unique_ptr<Sampler> Sampler::CreateSampler(Device& device_)
{
    return std::make_unique<Sampler>(device_);
}