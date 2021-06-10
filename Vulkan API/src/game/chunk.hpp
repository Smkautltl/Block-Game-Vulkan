#pragma once
#include "block.h"
#include "../core/game_object.h"
#include "../core/model.h"

namespace Vulkan
{
	class Chunk
	{
	public:

		Chunk(uint16_t id, int32_t y, int32_t z) : id_(id), y_(y), z_(z)
		{
			generate();
		}

		uint16_t id() { return id_; }
		std::pair<int32_t, int32_t> yz_Coords() { return std::make_pair(y_, z_); }

		void generate()
		{
			for (auto i = 0; i < chunk_size_; i++)
			{

			}
		}
		void load_game_objects(Device& device, std::vector<GameObject> &game_objects)
		{
			int counter = 0;
			for (auto block : blocks_)
			{
				std::vector<bool> faces;

				if (counter % 16 - 1 < 0)
				{
					faces.push_back(true);
				}
				else if (blocks_[counter % 16 - 1].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}

				if(counter % 16 + 1 > 15)
				{
					faces.push_back(true);
				}
				else if(blocks_[counter % 16 + 1].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}

				if(counter + 16 * 16 > 65280)
				{
					faces.push_back(true);
				}
				else if (blocks_[counter + 16 * 16].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}

				if (counter - 16 * 16 < 0)
				{
					faces.push_back(true);
				}
				else if (blocks_[counter - 16 * 16].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}

				if(0 <= counter % 256 && counter % 256 < 16)
				{
					faces.push_back(true);
				}
				else if (blocks_[counter - 16].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}

				if (240 <= counter % 256 && counter % 256 < 256)
				{
					faces.push_back(true);
				}
				else if (blocks_[counter + 16].get_block_type_() == 0)
				{
					faces.push_back(true);
				}
				else
				{
					faces.push_back(false);
				}
				
				block.regenerate_model(device, faces);

				if (block.Get() != nullptr)
				{
					blocks_to_render_.push_back(block);
				}
			}
		}

	private:
		int chunk_size_ = 16 * 16 * 255;
		std::vector<Block> blocks_;
		std::vector<Block> blocks_to_render_;
		
		uint16_t id_;
		int32_t y_;
		int32_t z_;
	};
}