#pragma once

	//-=-=-=-=- CORE -=-=-=-=-
#include "window.h"
#include "device.h"
#include "swapChain.h"

//-=-=-=-=- STD -=-=-=-=-
#include <memory>
#include <vector>

namespace Vulkan
{
	class Renderer
	{
	public:
		Renderer(Window &window, Device &device);
		~Renderer();

		//Deleting copy constructors
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass get_swap_chain_render_pass() const { return swap_chain_->get_render_pass(); }
		bool is_frame_in_progress() const { return is_frame_started_;  }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			VK_ASSERT(is_frame_started_, "Cannot get command buffer when frame not in progress!")
			return command_buffers_[current_frame_index_];
		}
 		
		VkCommandBuffer begin_frame();
		void end_frame();

		void begin_swap_chain_render_pass(VkCommandBuffer commandBuffer);
		void end_swap_chain_render_pass(VkCommandBuffer commandBuffer);

		int get_frame_index() const
		{
			VK_ASSERT(is_frame_started_, "Cannot get frame index when frame not in progress!")
			return current_frame_index_;
		};
	
	private:
		void create_command_buffers();
		void free_command_buffers();
		void recreate_swap_chain();

	public:


	private:
		Window& window_;
		Device& device_;
		std::unique_ptr<SwapChain> swap_chain_;
		std::vector<VkCommandBuffer> command_buffers_;

		uint32_t current_image_index_;
		int current_frame_index_{ 0 };
		bool is_frame_started_{ false };
	};
}