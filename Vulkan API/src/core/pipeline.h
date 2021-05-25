#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"

//-=-=-=-=- STD -=-=-=-=-
#include <string>
#include <vector>

namespace Vulkan
{
	struct pipelineConfigInfo
	{
		VkPipelineViewportStateCreateInfo viewport_info;
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
		VkPipelineRasterizationStateCreateInfo rasterization_info;
		VkPipelineMultisampleStateCreateInfo multisample_info;
		
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo color_blend_info;
		
		VkPipelineDepthStencilStateCreateInfo depth_stencil_info;

		std::vector<VkDynamicState> dynamic_state_enables;
		VkPipelineDynamicStateCreateInfo dynamic_state_info;
		
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pass = nullptr;
		uint32_t subpass = 0;
	};
	
	class pipeline
	{
	public:
		pipeline(Device &device, const std::string& vertPath, const std::string& fragPath, const pipelineConfigInfo& configInfo);
		~pipeline();

		pipeline(const pipeline&) = delete;
		pipeline &operator=(const pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		
		static pipelineConfigInfo defaultPipelineConfigInfo(pipelineConfigInfo &configInfo);
	
	private:
		static std::vector<char> readFile(const std::string& filePath);

		void create_graphics_pipeline(const std::string& vertPath, const std::string& fragPath, const pipelineConfigInfo& configInfo);
		void create_Shader_Module(const std::vector<char>& code, VkShaderModule* shaderModule);


	//-=-=-=-=- Variables -=-=-=-=-
	private:
		Device& device_;
		VkPipeline graphics_pipeline_{};
		VkShaderModule vert_shader_{};
		VkShaderModule frag_shader_{};
	};
}
