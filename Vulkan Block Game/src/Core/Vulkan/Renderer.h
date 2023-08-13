#pragma once
#include "Window.h"
#include "Device.h"
#include "Swapchain.h"
#include "Utils.h"
#include "DepthTexture.h"

//STD
#include <cassert>
#include <memory>
#include <vector>


class Renderer
{
public:
    Renderer(Window& window, Device& device);
    ~Renderer();
    
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void RecreateSwapchain();

    void UpdateCascades(FrameInfo& Frameinfo);

    VkCommandBuffer BeginFrame();
    void EndFrame();

    void BeginShadowmapRenderpass(VkCommandBuffer commandBuffer, uint8_t CascadeIndex);
    void EndShadowmapRenderpass(VkCommandBuffer commandBuffer);

    void BeginSwapchainRenderpass(VkCommandBuffer commandBuffer);
    void EndSwapchainRenderpass(VkCommandBuffer commandBuffer);

    VkRenderPass GetSwapchainRenderPass() const { return swapChain->GetRenderPass(); }
    VkRenderPass GetShadowRenderPass() const { return ShadowMap->GetRenderPass(); }
    VkCommandBuffer GetCurrentCommandBuffer() const 
    { 
        VK_CORE_ASSERT(isFrameStarted, "Cannot get command buffer when frame is not in progress!");
        return commandBuffers[currentFrameIndex];
    }

    int GetFrameIndex() const
    {
        VK_CORE_ASSERT(isFrameStarted, "Cannot get frame index when frame not in progress")
        return currentFrameIndex;
    }
    bool IsFrameInProgress() { return isFrameStarted; }
    float GetAspectRatio() const { return swapChain->ExtentAspectRatio(); }
    uint16_t GetSwapchainWidth() const { return swapChain->GetSwapChainExtent().width; }
    uint16_t GetSwapchainHeight() const { return swapChain->GetSwapChainExtent().height; }

    std::unique_ptr<DepthTexture>& GetShadowMap() { return ShadowMap; }

private:
    void createCommandBuffers();
    void freeCommandBuffers();
 
public:

private:
    Window& window;
    Device& device;
    
    std::unique_ptr<Swapchain> swapChain;  
    std::vector<VkCommandBuffer> commandBuffers;

    std::unique_ptr<DepthTexture> ShadowMap;

    uint32_t currentImageIndex = 0;
    int currentFrameIndex = 0;
    bool isFrameStarted = false;
};