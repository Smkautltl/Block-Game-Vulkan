#pragma once
//-=-=-=-=- STD -=-=-=-=-
#include <cstdint>

namespace Vulkan
{
	class Block
	{
	public:
		Block(){};
		Block(uint32_t id, uint32_t blockType) : block_type_(blockType), id_(id){}
		
		uint32_t block_type_ = 0;
	
	private:

		uint32_t id_ = 0;	
	};
}
