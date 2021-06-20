#define VMA_IMPLEMENTATION
#include "device.h"

//-=-=-=-=- STD -=-=-=-=-
#include <cstring>
#include <set>
#include <unordered_set>

namespace Vulkan
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            VK_CORE_TRACE(pCallbackData->pMessage)
            break;
			
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            VK_CORE_INFO(pCallbackData->pMessage)
            break;
			
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            VK_CORE_WARN(pCallbackData->pMessage)
            break;
			
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            VK_CORE_ERROR(pCallbackData->pMessage)
            break;
			
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            VK_CORE_INFO(pCallbackData->pMessage)
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
        else
        {
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

    Device::Device(Window &window) : window_{window}
	{
        create_instance();
        setup_debug_messenger();
        create_surface();
        pick_physical_device();
        create_logical_device();
        create_command_pool();
        setup_vma_allocator();
	}   
    Device::~Device()
	{
        VK_CORE_WARN("Device destructor called!")
        vmaDestroyAllocator(allocator);
        vkDestroyCommandPool(device_, command_pool_, nullptr);
        vkDestroyDevice(device_, nullptr);
        
        if (enableValidationLayers) 
        {
			DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
        }

        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkDestroyInstance(instance_, nullptr);
	}

//-=-=-=-=- INSTANCE -=-=-=-=--=-=-=-=-=-=-=-=-
	
    void Device::create_instance()
	{
        if (enableValidationLayers && !check_validation_layer_support()) 
        {
          VK_CORE_RUNTIME("Validation layers requested, but not available!");
		}

    	//Optional - But can provide the driver with info the better help optimize the application
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Block Game";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

    	//Tells vulkan what extensions and validation layers we what to use
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = get_required_extensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

    	//Enables the validation layers that we what to use
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
            createInfo.ppEnabledLayerNames = validation_layers_.data();

            populate_debug_messenger_create_info(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
    	else 
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

    	//Creates the instance and throws a runtime error if it fails to create one
        if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to create instance!");
        }

        VK_CORE_INFO("Instance Created!")
        has_gflw_required_instance_extensions();
}
    bool Device::check_validation_layer_support()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validation_layers_)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
    std::vector<const char*> Device::get_required_extensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    void Device::has_gflw_required_instance_extensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        VK_CORE_INFO("Available Extensions:")

            std::unordered_set<std::string> available;
        for (const auto& extension : extensions)
        {
            VK_CORE_INFO("\t\t{0}", extension.extensionName)
                available.insert(extension.extensionName);
        }

        VK_CORE_INFO("Required Extensions:")
            auto requiredExtensions = get_required_extensions();
        for (const auto& required : requiredExtensions)
        {
            VK_CORE_INFO("\t\t{0}", required)

                if (available.find(required) == available.end())
                {
                    VK_CORE_RUNTIME("Missing required glfw extension")
                }
        }
    }
	
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-=-=-=-=- DEBUG MESSENGER -=-=-=-=-=-=-=-=-=-
	
    void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;  // Optional
    }
    void Device::setup_debug_messenger()
    {
        if (!enableValidationLayers) return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populate_debug_messenger_create_info(createInfo);
        if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debug_messenger_) != VK_SUCCESS)
        {
            VK_CORE_RUNTIME("Failed to set up debug messenger!");
        }
        VK_CORE_INFO("Setup Debug Messenger!")
    }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Calls for the window to create a surface and stores the result
    void Device::create_surface()
    {
        window_.create_window_surface(instance_, &surface_);
    }
	
//-=-=-=-=- PHYSICAL & LOGICAL DEVICE -=-=-=-=-
	
    void Device::pick_physical_device()
	{
        uint32_t DeviceCount = 0;
        vkEnumeratePhysicalDevices(instance_, &DeviceCount, nullptr);

        if (DeviceCount == 0) 
        {
			VK_CORE_RUNTIME("Failed to find GPUs with Vulkan support!");
        }

    	VK_CORE_INFO("Device Count: {0}", DeviceCount)
    	
        std::vector<VkPhysicalDevice> Devices(DeviceCount);
        vkEnumeratePhysicalDevices(instance_, &DeviceCount, Devices.data());

        for (const auto &Device : Devices) 
        {
        	//Finds the first suitable device then stops checking for other devices
        	//TODO: Implement a scoring system to pick the best device instead of the first one that matches the requirements
			if (is_device_suitable(Device)) 
            {
                physical_device_ = Device;
                break;
			}
        }

        if (physical_device_ == VK_NULL_HANDLE) 
        {
			VK_CORE_RUNTIME("Failed to find a suitable GPU!");
        }

        vkGetPhysicalDeviceProperties(physical_device_, &properties);
        VK_CORE_INFO("Physical Device: {0}", properties.deviceName)
	}
    bool Device::is_device_suitable(VkPhysicalDevice Device)
    {
        QueueFamilyIndices indices = find_queue_families(Device);

        bool extensionsSupported = check_device_extension_support(Device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = query_swap_chain_support(Device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(Device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }
    bool Device::check_device_extension_support(VkPhysicalDevice Device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(device_extensions_.begin(), device_extensions_.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
    SwapChainSupportDetails Device::query_swap_chain_support(VkPhysicalDevice Device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, surface_, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, surface_, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(Device, surface_, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, surface_, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(Device, surface_, &presentModeCount, details.presentModes.data());
        }
        return details;
    }
	
    QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice Device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
        	
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }

        	//Checks to see if presentation is supported by the device being used
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, surface_, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport)
            {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }
            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }
	
    void Device::create_logical_device()
	{
        QueueFamilyIndices indices = find_queue_families(physical_device_);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        float queuePriority = 1.0f;
    	
        //Tells vulkan how many queues we want per queue family
        for (uint32_t queueFamily : uniqueQueueFamilies) 
        { 	
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

    	//Sets the device features that we will be using
        VkPhysicalDeviceFeatures DeviceFeatures = {};
        DeviceFeatures.samplerAnisotropy = VK_TRUE;

    	//All the info needed to create a logical device
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &DeviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
        createInfo.ppEnabledExtensionNames = device_extensions_.data();

    	//Adds some extra info for the logical device if we have validation layers enabled
        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
            createInfo.ppEnabledLayerNames = validation_layers_.data();
        }
    	else 
        {
			createInfo.enabledLayerCount = 0;
        }

    	//Creates the logical device
        if (vkCreateDevice(physical_device_, &createInfo, nullptr, &device_) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to create logical Device!");
        }

        VK_CORE_INFO("Logical Device Created!")

    	//Stores the graphics and present queue handles for use later
        vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphics_queue_);
        vkGetDeviceQueue(device_, indices.presentFamily, 0, &present_queue_);
	}
	
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	
    void Device::create_command_pool()
	{
        QueueFamilyIndices queueFamilyIndices = find_physical_queue_families();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(device_, &poolInfo, nullptr, &command_pool_) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to create command pool!")
        }
        VK_CORE_INFO("Command Pool Created!")
	}

    void Device::setup_vma_allocator()
    {
        VmaAllocatorCreateInfo allocinfo{};
        allocinfo.physicalDevice = physical_device_;
        allocinfo.device = device_;
        allocinfo.instance = instance_;
        vmaCreateAllocator(&allocinfo, &allocator);
    }

    void Device::destroy_buffer(AllocatedBuffer buffer)
    {
            vmaDestroyBuffer(allocator, buffer.buffer_, buffer.allocation_); 
    }

    VkFormat Device::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
        for (VkFormat format : candidates) {
          VkFormatProperties props;
          vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);

          if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
          {
	          return format;
          }
          else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
          {
            return format;
          }
        }
        VK_CORE_RUNTIME("Failed to find supported format!");
}
          
    uint32_t Device::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_device_, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
             return i;
            }
        }

        VK_CORE_RUNTIME("Failed to find suitable memory type!");
	}
    
    void Device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, AllocatedBuffer &buffer, const std::vector<Vertex>& vertices)
	{
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;	
        bufferInfo.usage = usage;
        //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vmaallocInfo{};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    	
        if (vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo, &buffer.buffer_, &buffer.allocation_, nullptr) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to create vertex buffer!")
        }

        void* data;
        vmaMapMemory(allocator, buffer.allocation_, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(size));
        vmaUnmapMemory(allocator, buffer.allocation_);

        //VkMemoryRequirements memRequirements;
        //vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);
        //
        //VkMemoryAllocateInfo allocInfo{};
        //allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        //allocInfo.allocationSize = memRequirements.size;
        //allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);
        //
        //if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
        //{
		//	VK_CORE_RUNTIME("Failed to allocate vertex buffer memory!");
        //}
        //
        //vkBindBufferMemory(device_, buffer, bufferMemory, 0);
	}
    
    VkCommandBuffer Device::begin_single_time_commands()
	{
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = command_pool_;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
	}  
    void Device::end_single_time_commands(VkCommandBuffer commandBuffer)
	{
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphics_queue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue_);

        vkFreeCommandBuffers(device_, command_pool_, 1, &commandBuffer);
}
          
	void Device::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
        VkCommandBuffer commandBuffer = begin_single_time_commands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        end_single_time_commands(commandBuffer);
	} 
    void Device::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
	{
        VkCommandBuffer commandBuffer = begin_single_time_commands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    	end_single_time_commands(commandBuffer);
	}
    void Device::create_image_with_info(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
	{
        if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to create image!");
        }
        VK_CORE_INFO("vkImage Created!")
		
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device_, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to allocate image memory!");
        }

        if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) 
        {
			VK_CORE_RUNTIME("Failed to bind image memory!");
        }
	}
    
}