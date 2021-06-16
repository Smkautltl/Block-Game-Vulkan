#include "world.h"

Vulkan::world::world(Device& device_)
{
	for (auto i = 0; i < 5; i++)
	{
		for (auto j = 0; j < 1; j++)
		{
			Chunk chnk{0, i,j};
			chnk.load_game_objects(device_);
			chunks_.push_back(std::move(chnk));
		}
	}
}

void Vulkan::world::render(VkCommandBuffer commandBuffer, camera& cam, VkPipelineLayout& pipeline_layout_)
{
	for (auto& chunk : chunks_)
	{
		SimplePushConstantData push;
		push.color = glm::vec3{0.5f, 0.5f, 0.5f};
		push.transform = cam.UpdateModelView(chunk.transform_.mat4());

		vkCmdPushConstants(commandBuffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		chunk.get()->bind(commandBuffer);
		chunk.get()->draw(commandBuffer);
	}
}
