#pragma once
//-=-=-=-=- STD -=-=-=-=-
#include <string>

//-=-=-=-=- STB -=-=-=-=-

#include <stb_image.h>

//-=-=-=-=- CORE -=-=-=-
#include "device.h"
#include "swapChain.h"

namespace Vulkan
{
	class Texture
	{
	public:
		Texture(Device& device, SwapChain& swapChain) : device_(device), swap_chain_(swapChain){}
		~Texture();
		bool load_image_from_file(const char* filename);
		void create_texture_sampler();

	private:
		Device& device_;
		SwapChain& swap_chain_;
		
		AllocatedBuffer stagingBuffer;
		AllocatedImage newImage;
		VkImageView image_view_;
		VkSampler textureSampler;
	};
}
