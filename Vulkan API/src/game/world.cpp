#include "world.h"

Vulkan::world::world(Device& device_)
{
	for (auto i = 0; i < 5; i++)
	{
		for (auto j = 0; j < 1; j++)
		{
			std::chrono::time_point<std::chrono::system_clock> start, end;
			start = std::chrono::system_clock::now();

			Chunk chnk {0, i, j};
			chnk.load_game_objects(device_);
			chunks_.push_back(std::move(chnk));

			end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			VK_CORE_INFO("Chunk: {0}-{1} created in {2}ms", i, j, std::to_string(elapsed_seconds.count()*1000))
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
