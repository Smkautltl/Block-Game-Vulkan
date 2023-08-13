#pragma once
#include <vulkan/vulkan.h>

#include <memory>
#include <string>

#include "Device.h"

class Texture
{
public:
    Texture(Device& device, const std::string& textureFilepath);
    Texture(Device& device, const std::string(&filepaths)[6]);
    Texture(Device& device, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    VkImageView imageView() const { return textureImageView; }
    VkSampler sampler() const { return textureSampler; }
    VkImage GetImage() const { return textureImage; }
    VkImageView GetImageView() const { return textureImageView; }
    VkDescriptorImageInfo GetImageInfo() const { return descriptor; }
    VkImageLayout GetImageLayout() const { return textureLayout; }
    VkExtent3D GetExtent() const { return extent; }
    VkFormat GetFormat() const { return format; }

    void UpdateDescriptor();
    void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

    static std::unique_ptr<Texture> CreateTextureFromFile(Device& device, const std::string& filepath);
    static std::unique_ptr<Texture> CreateCubeMap(Device& device, const std::string(&filepaths)[6]);
    

private:
    void createTextureImage(const std::string& filepath);
    void createCubeMap(const std::string(&filepaths)[6]);
    
    void createTextureImageView(VkImageViewType viewType);
    void createTextureSampler();

    VkDescriptorImageInfo descriptor{};

    Device& device;
    VkImage textureImage = nullptr;
    VkDeviceMemory textureImageMemory = nullptr;
    VkImageView textureImageView = nullptr;
    VkSampler textureSampler = nullptr;
    VkFormat format;
    VkImageLayout textureLayout;
    uint32_t mipLevels{ 1 };
    uint32_t layerCount{ 1 };
    VkExtent3D extent{};
};

class Sampler
{
public:
    Sampler(Device& device);
    ~Sampler();

    static std::unique_ptr<Sampler> CreateSampler(Device& device);

    VkSampler Get() const { return sampler; }

private:

public:

private:
    Device& device;
    VkSampler sampler;
};