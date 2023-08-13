#include "PointLightSystem.h"

PointLightSystem::PointLightSystem(Device& device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device_)
{
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
}
PointLightSystem::~PointLightSystem()
{
    vkDestroyPipelineLayout(device.vkDevice(), pipelineLayout, nullptr);
}

void PointLightSystem::Update(FrameInfo& frameInfo, UniformBufferObject& UBO)
{
    auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.FrameTime, randRotation);

    int lightIndex = 0;
    for (auto& kv : frameInfo.GameObjects)
    {
        auto& obj = kv.second;
        if (obj.pointLight == nullptr) continue;

        VK_CORE_ASSERT(lightIndex < MAX_LIGHTS, "Too many point lights created!")

        obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

        UBO.PointLights[lightIndex].pos = glm::vec4(obj.transform.translation, 1.0f);
        UBO.PointLights[lightIndex].colour = glm::vec4(obj.colour, obj.pointLight->lightIntensity);
        lightIndex++;
        
    }
    UBO.NumLights = lightIndex;
}

void PointLightSystem::Render(FrameInfo& Frameinfo)
{
    std::map<float, GameObject::id_t> sorted;
    for (auto& kv : Frameinfo.GameObjects)
    {
        auto& obj = kv.second;
        if (obj.pointLight == nullptr) continue;

        auto offset = Frameinfo.CameraPosition - obj.transform.translation;
        float disSquared = glm::dot(offset, offset);
        sorted[disSquared] = obj.GetID();
    }

    pipeline->Bind(Frameinfo.CommandBuffer);
    vkCmdBindDescriptorSets(Frameinfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &Frameinfo.GlobalDescriptorSet, 0, nullptr);

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        auto& obj = Frameinfo.GameObjects.at(it->second);
      
        PointLightPushConstants push{};
        push.pos = glm::vec4(obj.transform.translation, 1.0f);
        push.colour = glm::vec4(obj.colour, obj.pointLight->lightIntensity);
        push.radius = obj.transform.scale.x;

        vkCmdPushConstants(Frameinfo.CommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);
        vkCmdDraw(Frameinfo.CommandBuffer, 6, 1, 0, 0);
    }
    
}

void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

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
void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
{
    VK_CORE_ASSERT(pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout")

    PipelineConfigInfo pipelineConfig {};
    Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
    Pipeline::EnableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.attributeDescriptions.clear();
    
    pipeline = std::make_unique<Pipeline>(device, "Shaders/PointLight.vert.spv", "Shaders/PointLight.frag.spv", pipelineConfig);
}