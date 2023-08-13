#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_set>

#include "Window.h"

static const int MAX_TEXTURES = 1024;

struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Device
{
public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
    const bool enableFPS = false;
#else
    const bool enableValidationLayers = true;
    const bool enableFPS = true;
#endif

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    Device(Window& Window);
    ~Device();

    Device(const Device&) = delete;
    void operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::string CallbackMessage;

        switch (messageType)
        {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            CallbackMessage = "GENERAL Validation layer:\n\t\t";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            CallbackMessage = "SPECIFICATION Validation layer:\n\t\t";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            CallbackMessage = "PERFORMANCE Validation layer:\n\t\t";
            break;
        default:
            CallbackMessage = "UNKNOWN Validation layer:\n\t\t";
            break;
        }

        CallbackMessage += (std::string)pCallbackData->pMessage;

        size_t pos;
        while ((pos = CallbackMessage.find("|")) != std::string::npos)
        {
            CallbackMessage.replace(pos, 2, "\n\t\t");
        }
        while ((pos = CallbackMessage.find(";")) != std::string::npos)
        {
            CallbackMessage.replace(pos, 1, "\n\t\t");
        }

        CallbackMessage += "\n";

        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            VK_CORE_TRACE(CallbackMessage)
                break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            VK_CORE_INFO(CallbackMessage)
                break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            VK_CORE_WARN(CallbackMessage)
                break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            VK_CORE_ERROR(CallbackMessage)
                break;
        default:
            VK_CORE_INFO(CallbackMessage)
                break;
        }

        return VK_FALSE;
    }
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VkCommandPool vkCommandPool() { return commandPool; }
    VkDevice vkDevice() { return device; }
    VkSurfaceKHR vkSurface() { return surface; }
    VkQueue vkGraphicsQueue() { return graphicsQueue; }
    VkQueue vkPresentQueue() { return presentQueue; }
    VkPhysicalDevice vkPhysicalDevice() { return physicalDevice; }

    SwapChainSupportDetails GetSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices FindPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindSupportedDepthFormat(bool checkSamplingSupport);

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

    void CreateImageWithInfo( const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void TransitionImageLayout(VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout,uint32_t mipLevels = 1,uint32_t layerCount = 1);

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

public:
    VkPhysicalDeviceProperties Properties;

private:
    Window& window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; 
    VkCommandPool commandPool;

    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

};