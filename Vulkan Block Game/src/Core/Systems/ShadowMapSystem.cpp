#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ShadowMapSystem.h"

ShadowMapSystem::ShadowMapSystem(Device& device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device_)
{
    VK_CORE_INFO("Creating Shadow Map System!")
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
}
ShadowMapSystem::~ShadowMapSystem()
{
    vkDestroyPipelineLayout(device.vkDevice(), pipelineLayout, nullptr);
}

void ShadowMapSystem::Render(FrameInfo& frameInfo, VkDescriptorSet& descriptorset, uint8_t cascadeIndex)
{
    pipeline->Bind(frameInfo.CommandBuffer);
    vkCmdBindDescriptorSets(frameInfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorset, 0, nullptr);

    for (auto& kv : frameInfo.GameObjects)
    {
        auto& obj = kv.second;
        if (obj.model == nullptr) continue;

        ShadowPushConstants push{};
        push.cascadeIndex = cascadeIndex;

        vkCmdPushConstants(frameInfo.CommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShadowPushConstants), &push);

        obj.model->Bind(frameInfo.CommandBuffer);
        obj.model->Draw(frameInfo.CommandBuffer);
    }
}

void ShadowMapSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ShadowPushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSetLayout };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device.vkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        VK_CORE_RUNTIME("failed to create pipeline layout!");
    }
}
void ShadowMapSystem::CreatePipeline(VkRenderPass renderPass)
{
    VK_CORE_ASSERT(pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout")

    PipelineConfigInfo pipelineConfig {};
    Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineConfig.colourBlendInfo.attachmentCount = 0;
    pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    //TODO: pipelineConfig.rasterizationInfo.depthClampEnable = 
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "Shaders/shadow.vert.spv", "Shaders/shadow.frag.spv", pipelineConfig);
}