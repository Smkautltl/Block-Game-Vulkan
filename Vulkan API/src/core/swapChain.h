#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"

//-=-=-=-=- VULKAN -=-=-=-=-
#include <vulkan/vulkan.h>

//-=-=-=-=- STD -=-=-=-=-
#include <memory>
#include <vector>

namespace Vulkan
{

    class SwapChain
	{
    //-=-=-=-=-=- Methods -=-=-=-=-=-
    public:
        SwapChain(Device &deviceRef, VkExtent2D windowExtent);
        SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
        ~SwapChain();
    
        SwapChain(const SwapChain &) = delete;
        void operator=(const SwapChain &) = delete;
    
        VkFramebuffer get_frame_buffer(int index) { return swap_chain_framebuffers_[index]; }
        VkRenderPass get_render_pass() { return render_pass_; }
        VkImageView get_image_view(int index) { return swap_chain_image_views_[index]; }
        size_t image_count() { return swap_chain_images_.size(); }
        VkFormat get_swap_chain_image_format() { return swap_chain_image_format_; }
        VkExtent2D get_swap_chain_extent() { return swap_chain_extent_; }
        uint32_t width() { return swap_chain_extent_.width; }
        uint32_t height() { return swap_chain_extent_.height; }
    
        float extent_aspect_ratio() { return static_cast<float>(swap_chain_extent_.width) / static_cast<float>(swap_chain_extent_.height); }
        VkFormat find_depth_format();
    
        VkResult acquire_next_image(uint32_t *imageIndex);
        VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool compare_swap_chain_formats(const SwapChain& swapChain) const
        {
            return (swapChain.swap_chain_depth_format == swap_chain_depth_format) && 
					(swapChain.swap_chain_image_format_ == swap_chain_image_format_);
        }
    
    private:
        void init();
        void create_swap_chain();
        void create_image_views();
        void create_depth_resources();
        void create_render_pass();
        void create_framebuffers();
        void create_sync_objects();
    
        VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);

    //-=-=-=-=-=- Variables -=-=-=-=-=-
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    private:
        VkFormat swap_chain_image_format_;
        VkFormat swap_chain_depth_format;
        VkExtent2D swap_chain_extent_{};
    
        std::vector<VkFramebuffer> swap_chain_framebuffers_;
        VkRenderPass render_pass_{};
    
        std::vector<VkImage> depth_images_;
        std::vector<VkDeviceMemory> depth_image_memory_;
        std::vector<VkImageView> depth_image_views_;
        std::vector<VkImage> swap_chain_images_;
        std::vector<VkImageView> swap_chain_image_views_;
    
        Device &device_;
        VkExtent2D window_extent_;
    
        VkSwapchainKHR swap_chain_{};
        std::shared_ptr<SwapChain> old_swap_chain_;
    	
        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;
        std::vector<VkFence> images_in_flight_;
        size_t current_frame_ = 0;
    };

}
