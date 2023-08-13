#include "Pipeline.h"
#include "Core/Game Components/Model.h"

Pipeline::Pipeline(Device& device, const std::string& vertShadpath, const std::string& fragShadpath, const PipelineConfigInfo& configInfo) : device(device)
{
    createGraphicsPipeline(vertShadpath, fragShadpath, configInfo);
}
Pipeline::~Pipeline() 
{
    vkDestroyShaderModule(device.vkDevice(), vertShaderModule, nullptr);
    vkDestroyShaderModule(device.vkDevice(), fragShaderModule, nullptr);
    vkDestroyPipeline(device.vkDevice(), graphicsPipeline, nullptr);
}

//Public
void Pipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
{
    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pScissors = nullptr;

    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    configInfo.colourBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colourBlendAttachmentInfo.blendEnable = VK_FALSE;
    configInfo.colourBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colourBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colourBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    configInfo.colourBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colourBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colourBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    configInfo.colourBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colourBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colourBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    configInfo.colourBlendInfo.attachmentCount = 1;
    configInfo.colourBlendInfo.pAttachments = &configInfo.colourBlendAttachmentInfo;
    configInfo.colourBlendInfo.blendConstants[0] = 0.0f;  // Optional
    configInfo.colourBlendInfo.blendConstants[1] = 0.0f;  // Optional
    configInfo.colourBlendInfo.blendConstants[2] = 0.0f;  // Optional
    configInfo.colourBlendInfo.blendConstants[3] = 0.0f;  // Optional

    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.front = {};  // Optional
    configInfo.depthStencilInfo.back = {};   // Optional

    configInfo.dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStates.data();
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStates.size());
    configInfo.dynamicStateInfo.flags = 0;

    configInfo.bindingDescriptions = Vertex::getBindingDescriptions();
    configInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
}
void Pipeline::EnableAlphaBlending(PipelineConfigInfo& configInfo)
{
    configInfo.colourBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colourBlendAttachmentInfo.blendEnable = VK_TRUE;
    configInfo.colourBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.colourBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    configInfo.colourBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;             
    configInfo.colourBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  
    configInfo.colourBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    configInfo.colourBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;             
}

void Pipeline::Bind(VkCommandBuffer commandBuffer) 
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

//Private
void Pipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) 
{
    VK_CORE_ASSERT( configInfo.pipelineLayout != VK_NULL_HANDLE, "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    VK_CORE_ASSERT( configInfo.renderPass != VK_NULL_HANDLE, "Cannot create graphics pipeline: no renderPass provided in configInfo");

    auto vertCode = readFile(vertFilepath);
    auto fragCode = readFile(fragFilepath);

    createShaderModule(vertCode, &vertShaderModule);
    createShaderModule(fragCode, &fragShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";
    shaderStages[0].flags = 0;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";
    shaderStages[1].flags = 0;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;

    auto& bindDescriptions = configInfo.bindingDescriptions;
    auto& attributeDescrpitions = configInfo.attributeDescriptions;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescrpitions.size());
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescrpitions.data();
    vertexInputInfo.pVertexBindingDescriptions = bindDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    pipelineInfo.pViewportState = &configInfo.viewportInfo;
    pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
    pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
    pipelineInfo.pColorBlendState = &configInfo.colourBlendInfo;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;

    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CORE_CHECK_RESULT(vkCreateGraphicsPipelines(device.vkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline))
}
void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device.vkDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) 
    {
        VK_CORE_RUNTIME("failed to create shader module");
    }
}

std::vector<char> Pipeline::readFile(const std::string& filename)
{    
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!file.is_open()) 
    {
        VK_CORE_RUNTIME("Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}
