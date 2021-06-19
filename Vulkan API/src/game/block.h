#pragma once
#include <cstdint>

namespace Vulkan
{
	class Block
	{
	public:
		Block();
		Block(uint32_t id, uint32_t blockType) : id_(id), block_type_(blockType){}
		
		uint32_t block_type_;
	
	private:

		uint32_t id_;
		
	};
}
