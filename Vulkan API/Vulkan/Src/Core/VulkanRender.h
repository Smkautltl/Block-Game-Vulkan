#pragma once

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 800;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

inline VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
inline void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        func(instance, debug_messenger, pAllocator);
    }
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
struct swap_chain_support_details
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

static std::vector<char> read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        VK_CORE_WARN("Failed to open file!");
        throw std::runtime_error("");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};
struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanRender
{
public:
  
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<VulkanRender*>(glfwGetWindowUserPointer(window));
        app->framebuffer_resized_ = true;
    }

    void cleanup(VkDevice& device, GLFWwindow* window);
    void create_instance();

    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void setup_debug_messenger();

    void create_surface(GLFWwindow* window);

    void pick_physical_device();

    void create_logical_device(VkDevice& device);

    void create_swap_chain(VkDevice& device, GLFWwindow* window);

    void create_image_views(VkDevice& device);

    void create_render_pass(VkDevice& device);
    void create_descriptor_set_layout(VkDevice& device);

    void create_graphics_pipeline(VkDevice& device);

    void create_framebuffers(VkDevice& device);

    void create_command_pool(VkDevice& device);
    void create_command_buffers(VkDevice& device);

    void create_sync_objects(VkDevice& device);

    void draw_frame(VkDevice& device, GLFWwindow* window);
	
    void update_uniform_buffer(VkDevice& device, uint32_t currentImage);

    void cleanup_swap_chain(VkDevice& device);
    void recreate_swap_chain(VkDevice& device, GLFWwindow* window);

    bool is_device_suitable(VkPhysicalDevice device);

    bool check_device_extension_support(VkPhysicalDevice device);

    bool check_validation_layer_support();

    template <typename t>
    void create_vk_buffer(VkDevice& device, const std::vector<t>& data, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkBufferUsageFlags usage);
	
    void create_vertex_buffer(VkDevice& device);
    void create_index_buffer(VkDevice& device);
    void create_uniform_buffers(VkDevice& device);
	
    void create_descriptor_pool(VkDevice& device);
    void create_descriptor_sets(VkDevice& device);
	
    void create_buffer(VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copy_buffer(VkDevice& device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


    VkShaderModule create_shader_module(VkDevice& device, const std::vector<char>& code);
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D choose_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
														 VkDebugUtilsMessageTypeFlagsEXT messageType, 
														 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
        std::string output = static_cast<std::string>(pCallbackData->pMessage);
    	
		switch (messageSeverity)
		{
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            VK_CORE_TRACE(output);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            VK_CORE_INFO(output);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            VK_CORE_WARN(output);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            VK_CORE_ERROR(output);
            break;
			
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            VK_CORE_TRACE(output);
            break;
			
        default: 
            VK_CORE_TRACE(output);
			break;
		}

        return VK_FALSE;
    }

    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

    std::vector<const char*> get_required_extensions();
	
    swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_Device_, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
public:

private:
    
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debug_messenger_{};
    VkSurfaceKHR surface_{};

    VkSwapchainKHR swap_chain_{};
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_{};
    std::vector<VkImageView> swap_chain_image_views_;
    std::vector<VkFramebuffer> swap_chain_frame_buffers_;

    VkRenderPass render_pass_{};
    VkPipelineLayout pipeline_layout_{};
    VkDescriptorSetLayout descriptor_set_layout_{};
    VkPipeline graphics_pipeline_{};

    VkPhysicalDevice physical_Device_ = VK_NULL_HANDLE;  

    VkQueue graphics_queue_{};
    VkQueue present_queue_{};

    VkCommandPool command_pool_{};
    std::vector<VkCommandBuffer> command_buffers_;

    std::vector<VkSemaphore> image_available_semaphore_;
    std::vector<VkSemaphore> render_finished_semaphore_;

    std::vector<VkFence> in_flight_fences_;
    std::vector<VkFence> images_in_flight_;
    size_t current_frame_ = 0;

    bool framebuffer_resized_ = false;

    const std::vector<Vertex> vertices_ = 
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices_ = {
        0, 1, 2,
    	2, 3, 0
    };
	
    VkBuffer vertex_buffer_;
    VkDeviceMemory vertex_buffer_memory_;
    VkBuffer index_buffer_;
    VkDeviceMemory index_buffer_memory_;

    std::vector<VkBuffer> uniform_buffers_;
    std::vector<VkDeviceMemory> uniform_buffers_memory_;

    VkDescriptorPool descriptor_pool_;
    std::vector<VkDescriptorSet> descriptor_sets_;
};