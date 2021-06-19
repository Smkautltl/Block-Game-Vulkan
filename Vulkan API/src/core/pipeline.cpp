#include "pipeline.h"

//-=-=-=-=- CORE -=-=-=-=-
#include "model.h"

//-=-=-=-=- STD -=-=-=-=-
#include <fstream>
#include <stdexcept>
#include <cassert>

namespace Vulkan
{
	pipeline::pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const pipelineConfigInfo& config_info) : device_(device)
	{
		create_graphics_pipeline(vertPath, fragPath, config_info);
	}
	pipeline::~pipeline()
	{
		vkDestroyShaderModule(device_.get_device(), vert_shader_, nullptr);
		vkDestroyShaderModule(device_.get_device(), frag_shader_, nullptr);
		
		vkDestroyPipeline(device_.get_device(), graphics_pipeline_, nullptr);
	}

	void pipeline::bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_); 
	}

	pipelineConfigInfo pipeline::defaultPipelineConfigInfo(pipelineConfigInfo& config_info)
	{
		config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

		config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config_info.rasterization_info.depthClampEnable = VK_FALSE;
		config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		config_info.rasterization_info.lineWidth = 1.0f;
		config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
		config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		config_info.rasterization_info.depthBiasEnable = VK_FALSE;
		config_info.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
		config_info.rasterization_info.depthBiasClamp = 0.0f;           // Optional
		config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

		config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config_info.viewport_info.viewportCount = 1;
		config_info.viewport_info.pViewports = nullptr;
		config_info.viewport_info.scissorCount = 1;
		config_info.viewport_info.pScissors = nullptr;
		
		config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config_info.multisample_info.sampleShadingEnable = VK_FALSE;
		config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info.multisample_info.minSampleShading = 1.0f;           // Optional
		config_info.multisample_info.pSampleMask = nullptr;             // Optional
		config_info.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
		config_info.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

		config_info.color_blend_attachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		config_info.color_blend_attachment.blendEnable = VK_FALSE;
		config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
		config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
		config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
		config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		config_info.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
		config_info.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
		config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
		config_info.depth_stencil_info.front = {};  // Optional
		config_info.depth_stencil_info.back = {};   // Optional

		config_info.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		config_info.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config_info.dynamic_state_info.pDynamicStates = config_info.dynamic_state_enables.data();
		config_info.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(config_info.dynamic_state_enables.size());
		config_info.dynamic_state_info.flags = 0;

		return config_info;
	}

	std::vector<char> pipeline::readFile(const std::string& filePath)
	{
		std::ifstream file{filePath, std::ios::ate | std::ios::binary};

		if(!file.is_open())
		{
			//std::string message = "Failed to open file: " + filePath;
			VK_CORE_RUNTIME("Failed to open file: " + filePath);
			//return std::vector<char>{};
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void pipeline::create_graphics_pipeline(const std::string& vertPath, const std::string& fragPath, const pipelineConfigInfo& config_info)
	{
		VK_CORE_ASSERT(config_info.pipeline_layout != VK_NULL_HANDLE, "Cannot create graphics pipeline :: no pipelineLayout provided  in configInfo")
		VK_CORE_ASSERT(config_info.render_pass != VK_NULL_HANDLE, "Cannot create graphics pipeline :: no renderPass provided  in configInfo")
		
		auto vertCode = readFile(vertPath);
		auto fragCode = readFile(fragPath);

		create_Shader_Module(vertCode, &vert_shader_);
		create_Shader_Module(fragCode, &frag_shader_);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vert_shader_;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;
		
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = frag_shader_;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		auto bindingdescriptions = Vertex::getBindingDescriptions();
		auto attributedescriptions = Vertex::getAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributedescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingdescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributedescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingdescriptions.data();


		VkPipelineColorBlendStateCreateInfo colourblendinfo{};
		colourblendinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colourblendinfo.logicOpEnable = VK_FALSE;
		colourblendinfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		colourblendinfo.attachmentCount = 1;
		colourblendinfo.pAttachments = &config_info.color_blend_attachment;
		colourblendinfo.blendConstants[0] = 0.0f;  // Optional
		colourblendinfo.blendConstants[1] = 0.0f;  // Optional
		colourblendinfo.blendConstants[2] = 0.0f;  // Optional
		colourblendinfo.blendConstants[3] = 0.0f;  // Optional
		
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &config_info.input_assembly_info;
		pipelineInfo.pViewportState = &config_info.viewport_info;
		pipelineInfo.pRasterizationState = &config_info.rasterization_info;
		pipelineInfo.pMultisampleState = &config_info.multisample_info;
		pipelineInfo.pColorBlendState = &colourblendinfo;
		pipelineInfo.pDepthStencilState = &config_info.depth_stencil_info;
		pipelineInfo.pDynamicState = &config_info.dynamic_state_info;
		
		pipelineInfo.layout = config_info.pipeline_layout;
		pipelineInfo.renderPass = config_info.render_pass;
		pipelineInfo.subpass = config_info.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if(vkCreateGraphicsPipelines(device_.get_device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline_) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to create graphics pipelines!");
		}
		VK_CORE_INFO("Graphics Pipeline Created!")
	}

	void pipeline::create_Shader_Module(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if(vkCreateShaderModule(device_.get_device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to create shader module!")
		}
		VK_CORE_INFO("Shader Module Created!")
	}
}
