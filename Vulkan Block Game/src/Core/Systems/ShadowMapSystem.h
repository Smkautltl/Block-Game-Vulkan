#pragma once
#include "Core/Vulkan/Device.h"
#include "Core/Vulkan/Pipeline.h"
#include "Core/Vulkan/Utils.h"

struct ShadowPushConstants
{
    glm::vec4 position{0.f};
    uint8_t cascadeIndex;
};

class ShadowMapSystem
{
public:
    ShadowMapSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~ShadowMapSystem();

    ShadowMapSystem(const ShadowMapSystem&) = delete;
    ShadowMapSystem& operator=(const ShadowMapSystem&) = delete;

    void Render(FrameInfo& frameInfo, VkDescriptorSet& descriptorset, uint8_t cascadeIndex);

private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;

};