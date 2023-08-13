#pragma once
#include "Core/Vulkan/Device.h"
#include "Core/Vulkan/Pipeline.h"
#include "Core/Vulkan/Utils.h"

#include <map>

struct PointLightPushConstants
{
    glm::vec4 pos{};
    glm::vec4 colour{};
    float radius;
};

class PointLightSystem
{
public:
    PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem&) = delete;
    PointLightSystem& operator=(const PointLightSystem&) = delete;

    void Update(FrameInfo& frameInfo, UniformBufferObject& UBO);
    void Render(FrameInfo& frameInfo);

private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;

    glm::vec3 randRotation{ 0, -1, 0 };

};
