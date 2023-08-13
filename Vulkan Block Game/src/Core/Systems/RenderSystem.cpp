#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "RenderSystem.h"

RenderSystem::RenderSystem(Device& device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device_)
{
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
}
RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(device.vkDevice(), pipelineLayout, nullptr);
}

void RenderSystem::Render(FrameInfo& Frameinfo)
{
    pipeline->Bind(Frameinfo.CommandBuffer);
    vkCmdBindDescriptorSets(Frameinfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &Frameinfo.GlobalDescriptorSet, 0, nullptr);

    for (auto& kv : Frameinfo.GameObjects)
    {
        auto& obj = kv.second;
        if (obj.model == nullptr) continue;

        PushConstants push{};
        push.normalMatrix = obj.transform.normalMatrix();
        push.modelMatrix = obj.transform.mat4();

        vkCmdPushConstants(Frameinfo.CommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants), &push);

        obj.model->Bind(Frameinfo.CommandBuffer);
        obj.model->Draw(Frameinfo.CommandBuffer);
    }
}

void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSetLayout};

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
void RenderSystem::CreatePipeline(VkRenderPass renderPass)
{
    VK_CORE_ASSERT(pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout")

        PipelineConfigInfo pipelineConfig {};
    Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "Shaders/shader.vert.spv", "Shaders/shader.frag.spv", pipelineConfig);
}