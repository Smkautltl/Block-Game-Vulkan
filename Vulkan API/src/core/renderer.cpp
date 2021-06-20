#include "Renderer.h"

//-=-=-=-=- STD -=-=-=-=-
#include <array>
#include <stdexcept>

namespace Vulkan
{

	Renderer::Renderer(Window& window, Device& device) : window_(window), device_(device)
	{
		recreate_swap_chain();
		create_command_buffers();
	}

	Renderer::~Renderer()
	{
		VK_CORE_WARN("Rnderer destructor called!");
		free_command_buffers();
	}

	void Renderer::recreate_swap_chain()
	{
		auto extent = window_.get_extent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = window_.get_extent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device_.get_device());

		if (swap_chain_ == nullptr)
		{
			swap_chain_ = std::make_unique<SwapChain>(device_, extent);
		}
		else
		{
			std::shared_ptr<SwapChain> oldSwapChain = std::move(swap_chain_);		
			swap_chain_ = std::make_unique<SwapChain>(device_, extent, oldSwapChain);

			if(!oldSwapChain->compare_swap_chain_formats(*swap_chain_))
			{
				VK_CORE_RUNTIME("Swap chain image format has changed!");
			}
		}

	}

	VkCommandBuffer Renderer::begin_frame()
	{
		VK_ASSERT(!is_frame_in_progress(), "Can't call begin_frame while already in progress!")

		auto result = swap_chain_->acquire_next_image(&current_image_index_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate_swap_chain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			VK_CORE_RUNTIME("Failed to acquire swap chain image!");
		}

		is_frame_started_ = true;

		auto  commandBuffer = getCurrentCommandBuffer();		
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &begin_info) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to begin command buffer!");
		}

		return commandBuffer;
	}

	void Renderer::end_frame()
	{
		VK_ASSERT(is_frame_started_, "Can't call end_frame while frame is not in progress!")

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to record command buffer!");
		}

		auto result = swap_chain_->submit_command_buffers(&commandBuffer, &current_image_index_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.was_window_resized())
		{
			window_.reset_window_resize_flag();
			recreate_swap_chain();
		}
		else if (result != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to submit swap chain command buffer!");
		}

		is_frame_started_ = false;
		current_frame_index_ = (current_frame_index_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
		
	}

	void Renderer::begin_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		VK_ASSERT(is_frame_started_, "Can't call begin_swap_chain_render_pass if frame is not in progress!")
		VK_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't begin render pass on command buffer from a different frame!")

		VkRenderPassBeginInfo render_pass_info {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = swap_chain_->get_render_pass();
		render_pass_info.framebuffer = swap_chain_->get_frame_buffer(current_image_index_);

		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = swap_chain_->get_swap_chain_extent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f, 0 };

		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swap_chain_->get_swap_chain_extent().width);
		viewport.height = static_cast<float>(swap_chain_->get_swap_chain_extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ {0, 0}, swap_chain_->get_swap_chain_extent() };

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::end_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		VK_ASSERT(is_frame_started_, "Can't call end_swap_chain_render_pass if frame is not in progress!")
		VK_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't end render pass on command buffer from a different frame!")

		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::create_command_buffers()
	{
		command_buffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device_.get_command_pool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

		if (vkAllocateCommandBuffers(device_.get_device(), &allocInfo, command_buffers_.data()) != VK_SUCCESS)
		{
			VK_CORE_RUNTIME("Failed to allocate command buffers!");
		}
	}

	void Renderer::free_command_buffers()
	{
		vkFreeCommandBuffers(device_.get_device(), device_.get_command_pool(), static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
		command_buffers_.clear();
	}
}

