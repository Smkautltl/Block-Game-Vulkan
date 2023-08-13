#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Device.h"

static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
#define SHADOWMAP_SIZE 2048

class Swapchain
{
public:

    Swapchain(Device& device_, VkExtent2D windowExtent);
    Swapchain(Device& device_, VkExtent2D windowExtent, std::shared_ptr<Swapchain> previous);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass GetRenderPass() { return renderPass; }
    VkImageView GetImageView(int index) { return swapChainImageViews[index]; }
    size_t ImageCount() { return swapChainImages.size(); }
    VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
    VkExtent2D GetSwapChainExtent() { return swapChainExtent; }
    uint32_t Width() { return swapChainExtent.width; }
    uint32_t Height() { return swapChainExtent.height; }

    float ExtentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }
    VkFormat FindDepthFormat();

    VkResult AcquireNextImage(uint32_t* imageIndex);
    VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    bool CompareSwapFormats(const Swapchain& swapchain) const
    {
        return swapchain.swapChainDepthFormat == swapChainDepthFormat && swapchain.swapChainImageFormat == swapChainImageFormat;
    }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

public:

private:
    Device& device;

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    VkSampler depthSampler;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkExtent2D windowExtent;
    VkSwapchainKHR swapChain;
    std::shared_ptr<Swapchain> oldSwapchain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};