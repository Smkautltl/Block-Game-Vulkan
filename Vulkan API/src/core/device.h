#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "Window.h"

//-=-=-=-=- VMA -=-=-=-=-
#include "vk_mem_alloc.h"

//-=-=-=-=- STD -=-=-=-=-
#include <vector>

//-=-=-=-=- GLM -=-=-=-=-
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Vulkan
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 colour;
        glm::vec3 normal;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

	struct SwapChainSupportDetails
	{
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };
    
    struct QueueFamilyIndices
	{
      uint32_t graphicsFamily;
      uint32_t presentFamily;
      bool graphicsFamilyHasValue = false;
      bool presentFamilyHasValue = false;
      bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    struct AllocatedBuffer
    {
        VkBuffer buffer_;
        VmaAllocation allocation_;
    };
    struct AllocatedImage {
        VkImage _image;
        VmaAllocation _allocation;
    };

    class Device
	{
    public:
        #ifdef NDEBUG
          const bool enableValidationLayers = false;
        #else
          const bool enableValidationLayers = true;
        #endif
    
        Device(Window &window);
        ~Device();
    
        // Not copyable or movable
        Device(const Device&) = delete;
        void operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device &operator=(Device&&) = delete;
    
        VkCommandPool get_command_pool() const { return command_pool_; }
        VkDevice get_device() const { return device_; }
        VkSurfaceKHR get_surface() const { return surface_; }
        VkQueue graphics_queue() const { return graphics_queue_; }
        VkQueue present_queue() const { return present_queue_; }
        VmaAllocator get_allocator() const { return allocator; }
    
        SwapChainSupportDetails get_swap_chain_support() { return query_swap_chain_support(physical_device_); }
        uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices find_physical_queue_families() { return find_queue_families(physical_device_); }
        VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    
        // Buffer Helper Functions
        void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, AllocatedBuffer& buffer, const std::vector<Vertex>& vertices);
        AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memUsage, VkDeviceSize imageSize, void* pixels);
        void destroy_buffer(AllocatedBuffer buffer);
        VkCommandBuffer begin_single_time_commands();
        void end_single_time_commands(VkCommandBuffer commandBuffer);
        void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
    
        void create_image_with_info(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    	void create_image(VkImageCreateInfo& imgInfo, VmaMemoryUsage memUsage, AllocatedImage& imageAlloc);
        void destroy_image(AllocatedImage image);
        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    	
    
    private:
        void create_instance();
        void setup_debug_messenger();
        void create_surface();
        void pick_physical_device();
        void create_logical_device();
        void create_command_pool();
        void setup_vma_allocator();

    	
        // Helper Functions
        bool is_device_suitable(VkPhysicalDevice device);
        std::vector<const char *> get_required_extensions();
        bool check_validation_layer_support();
        QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
        void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void has_gflw_required_instance_extensions();
        bool check_device_extension_support(VkPhysicalDevice device);
        SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

    //-=-=-=-=-=-=-=-=- Variables -=-=-=-=-=-=-=-=-
    public:
        VkPhysicalDeviceProperties properties{};
        VmaAllocator allocator;
        std::mutex allocMutex;
    
    private:
        VkInstance instance_{};
        VkDebugUtilsMessengerEXT debug_messenger_{};
        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
        Window &window_;
        VkCommandPool command_pool_{};
    
        VkDevice device_{};
        VkSurfaceKHR surface_{};
        VkQueue graphics_queue_{};
        VkQueue present_queue_{};
    
        const std::vector<const char *> validation_layers_ = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> device_extensions_ = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}
