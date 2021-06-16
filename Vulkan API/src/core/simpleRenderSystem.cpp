#include "simpleRenderSystem.h"

//-=-=-=-=- GLFW -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//-=-=-=-=- STD -=-=-=-=-
#include <stdexcept>

namespace Vulkan
{
	//struct SimplePushConstantData
	//{
	//	glm::mat4 transform{ 1.0f };
	//	alignas(16) glm::vec3 color;
	//};

	SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass) : device_(device)
	{
		create_pipeline_layout();
		create_pipeline(renderPass);
		world_ = new world{device};
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(device_.get_device(), pipeline_layout_, nullptr);
	}

	void SimpleRenderSystem::create_pipeline_layout()
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pSetLayouts = nullptr;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;

		if (vkCreatePipelineLayout(device_.get_device(), &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to create pipeline layout!");
		}
	}
	void SimpleRenderSystem::create_pipeline(VkRenderPass renderPass)
	{
		VK_CORE_ASSERT(pipeline_layout_ != nullptr, "Cannot create pipeline before pipeline layout!")

		pipelineConfigInfo pipeline_config_info {};
		auto pipeline_config = pipeline::defaultPipelineConfigInfo(pipeline_config_info);
		pipeline_config.render_pass = renderPass;
		pipeline_config.pipeline_layout = pipeline_layout_;
		pipeline_ = std::make_unique<pipeline>(device_, "Shaders/vert.spv", "Shaders/frag.spv", pipeline_config);
	}

	void SimpleRenderSystem::render_game_objects(VkCommandBuffer commandbuffer, camera& cam)
	{

		pipeline_->bind(commandbuffer);

		world_->render(commandbuffer, cam, pipeline_layout_);
	}

}
