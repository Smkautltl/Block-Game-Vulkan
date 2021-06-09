#pragma once
#include "cube.h"
#include "../core/game_object.h"
#include "../core/model.h"

namespace Vulkan
{
	class chunk
	{
	public:

		void load_game_objects(Device& device, std::vector<Block> blockTypes, std::vector<GameObject> &game_objects)
		{
			auto x_counter = 0;
			auto y_counter = 0;

			for (auto i = 0; i < chunk_size_; i++)
			{
				auto block = GameObject::createGameObject();
				
				const auto layer = i / 255;
				block.transform_.translation = { x_counter, y_counter, layer };

				std::vector<int> facesToCull = { x_counter, x_counter-15 , layer, layer-255, y_counter, y_counter-15 };
				
				if (facesToCull[0] != 0 && facesToCull[1] != 0 && facesToCull[2] != 0 && facesToCull[3] != 0 && facesToCull[4] != 0 && facesToCull[5] != 0 )
				{
					
				}
				else
				{
					
					block.model = blockTypes[rand() % 2].GetCulled(device, facesToCull);

					
					if (x_counter == 15)
					{
						x_counter = 0;
						y_counter++;
					}
					else
					{
						x_counter++;
					}
					if (y_counter == 15)
					{
						y_counter = 0;
					}

					
					block.transform_.scale = { 1.f, 1.f, 1.f };
					game_objects.push_back(std::move(block));
				}
			}
		}

	private:
		int chunk_size_ = 16 * 16 * 255;
		
	};
	
}
