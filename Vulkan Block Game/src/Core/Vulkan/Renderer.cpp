#include "Renderer.h"

Renderer::Renderer(Window& Window, Device& Device) : window(Window), device(Device)
{
    VK_CORE_INFO("Creating Renderer!")
    ShadowMap = DepthTexture::CreateDepthTexture(device);
    RecreateSwapchain();
    createCommandBuffers();
}
Renderer::~Renderer()
{
    freeCommandBuffers();
}

//Public
VkCommandBuffer Renderer::BeginFrame()
{
    VK_CORE_ASSERT(!isFrameStarted, "A frame is already in progress")

    auto result = swapChain->AcquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        VK_CORE_RUNTIME("failed to acquire swap chain image");
    }

    isFrameStarted = true;
    auto commandBuffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to begin recording command buffer!");
    }
    
    return commandBuffer;
}
void Renderer::EndFrame()
{
    VK_CORE_ASSERT(isFrameStarted, "Cannot call endFrame when frame is not in progress!");
    auto commandBuffer = GetCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to record command buffer!");
    }

    auto result = swapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
    {
        window.ResetWindowResizedFlag();
        RecreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to present swap chain image!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::BeginShadowmapRenderpass(VkCommandBuffer commandBuffer, uint8_t CascadeIndex)
{
    VK_CORE_INFO("Began Shadowmap[{0}] Renderpass!", CascadeIndex)

    VkClearValue clearValues[1];
    clearValues[0].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = ShadowMap->GetRenderPass();
    renderPassBeginInfo.renderArea.extent.width = SHADOWMAP_SIZE;
    renderPassBeginInfo.renderArea.extent.height = SHADOWMAP_SIZE;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = clearValues;

    VkViewport viewport{};
    viewport.width = SHADOWMAP_SIZE;
    viewport.height = SHADOWMAP_SIZE;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent.width = SHADOWMAP_SIZE;
    scissor.extent.height = SHADOWMAP_SIZE;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    renderPassBeginInfo.framebuffer = ShadowMap->GetCascades()[CascadeIndex].FrameBuffer;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
}

void Renderer::EndShadowmapRenderpass(VkCommandBuffer commandBuffer)
{
    VK_CORE_INFO("Finished Shadowmap Renderpass!")
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::BeginSwapchainRenderpass(VkCommandBuffer commandBuffer)
{
    VK_CORE_INFO("Began Renderpass!")

    VK_CORE_ASSERT(isFrameStarted, "Cannot call BeginSwapchainRenderpass when frame is not in progress!");
    VK_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Command buffer given to BeginSwapchainRenderpass is not the same as CurrentCommandBuffer")

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->GetRenderPass();
    renderPassInfo.framebuffer = swapChain->GetFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{ {0, 0}, swapChain->GetSwapChainExtent() };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void Renderer::EndSwapchainRenderpass(VkCommandBuffer commandBuffer)
{
    VK_CORE_ASSERT(isFrameStarted, "Cannot call EndSwapchainRenderpass when frame is not in progress!");
    VK_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Command buffer given to EndSwapchainRenderpass is not the same as CurrentCommandBuffer")

    VK_CORE_INFO("Finished Renderpass!")
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::RecreateSwapchain()
{
    auto extent = window.GetExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        //TODO: Fix bug where application locks up when resizing the window vertically to 0 pixels
        extent = window.GetExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.vkDevice());

    if (swapChain == nullptr)
    {
        swapChain = std::make_unique<Swapchain>(device, extent);
    }
    else
    {
        std::shared_ptr<Swapchain> oldSwapChain = std::move(swapChain);
        swapChain = std::make_unique<Swapchain>(device, extent, oldSwapChain);

        if (!oldSwapChain->CompareSwapFormats(*swapChain.get()))
        {
            VK_CORE_RUNTIME("Swapchain image(or depth) format has changed!")
        }
    }
}

void Renderer::UpdateCascades(FrameInfo& Frameinfo)
{
    float cascadeSplits[SHADOWMAP_CASCADE_COUNT];

    float nearClip = 0.1f;
    float farClip = 1500.f;
    float clipRange = farClip - nearClip;

    float minZ = nearClip;
    float maxZ = nearClip + clipRange;

    float range = maxZ - minZ;
    float ratio = maxZ / minZ;

    for (uint32_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
    {
        float p = (i + 1) / static_cast<float>(SHADOWMAP_CASCADE_COUNT);
        float log = minZ * std::pow(ratio, p);
        float uniform = minZ + range * p;
        float d = 0.95f * (log - uniform) + uniform;
        cascadeSplits[i] = (d - nearClip) / clipRange;
    }

    float lastSplitDist = 0.0;
    for (uint32_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
    {
        float splitDist = cascadeSplits[i];

        glm::vec3 frustumCorners[8] = 
        {
            glm::vec3(-1.0f,  1.0f, 0.0f),
            glm::vec3(1.0f,  1.0f, 0.0f),
            glm::vec3(1.0f, -1.0f, 0.0f),
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
        };

        // Project frustum corners into world space
        glm::mat4 invCam = glm::inverse(Frameinfo.Projection * Frameinfo.View);
        for (uint32_t i = 0; i < 8; i++)
        {
            glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
            frustumCorners[i] = invCorner / invCorner.w;
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
            frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
            frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
        }

        // Get frustum center
        glm::vec3 frustumCenter = glm::vec3(0.0f);
        for (uint32_t i = 0; i < 8; i++)
        {
            frustumCenter += frustumCorners[i];
        }
        frustumCenter /= 8.0f;

        float radius = 0.0f;
        for (uint32_t i = 0; i < 8; i++)
        {
            float distance = glm::length(frustumCorners[i] - frustumCenter);
            radius = glm::max(radius, distance);
        }
        radius = std::ceil(radius * 16.0f) / 16.0f;

        glm::vec3 maxExtents = glm::vec3(radius);
        glm::vec3 minExtents = -maxExtents;

        glm::vec3 lightDir = normalize(-Frameinfo.SunPosition);
        glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

        // Store split distance and matrix in cascade
        ShadowMap->GetCascades()[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
        ShadowMap->GetCascades()[i].ViewProjMatrix = lightOrthoMatrix * lightViewMatrix;

        lastSplitDist = cascadeSplits[i];
    }
}

//Private
void Renderer::createCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.vkCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.vkDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to allocate command buffers!");
    }

}
void Renderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(device.vkDevice(), device.vkCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
}
