#pragma once
#include "block.h"
//#include "../core/game_object.h"
#include <fstream>

#include "../core/model.h"

namespace Vulkan
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation;

		glm::mat4 mat4()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			return glm::mat4
			{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
		}
	};

	class Chunk
	{
	public:

		Chunk(uint16_t id, int32_t x, int32_t z) : id_(id), x_(x), z_(z)
		{
			transform_.translation = {(x*16)+1, 0, (z*16)+1};
			generate();
		}

		uint16_t id() { return id_; }
		std::pair<int32_t, int32_t> xz_Coords() { return std::make_pair(x_, z_); }
		std::shared_ptr<Model> get() { return model; }
	
		void generate()
		{	
			unsigned int xcounter = 0;
			unsigned int ycounter = 0;
			unsigned int zcounter = 0;
			
			for (uint32_t i = 0; i < chunk_size_; i++)
			{
				unsigned int id;
				unsigned int blocktype;
				
				if (ycounter < 70)
				{
					id = i;
					blocktype = 3;
				}
				else if (ycounter < 74)
				{
					id = i;
					blocktype = 2;
				}
				else if (ycounter < 75)
				{
					id = i;
					blocktype = 1;
				}
				else
				{
					id = i;
					blocktype = 0;
				}

				Block newBlock{ id, blocktype, {xcounter, ycounter, zcounter}};
				blocks_.push_back(std::move(newBlock));
				
				if (xcounter == 15)
				{
					xcounter = 0;
					zcounter++;
				}
				else
				{
					xcounter++;
				}
				if (zcounter > 15)
				{
					zcounter = 0;
					ycounter++;
				}
			}
		}
		
		void load_game_objects(Device& device)
		{
			
			std::vector<Model::Vertex> vertices;
			int32_t counter = 0;
			unsigned int xcounter = 0;
			unsigned int ycounter = 0;
			unsigned int zcounter = 0;

			//Checks all blocks to see what faces need to be rendered
			for (auto block : blocks_)
			{
				//Left Face
				if (xcounter == 0 || blocks_[(counter % 16) - 1].block_type_ == 0)
					block.faces[0] = true;
				
				//Right Face
				if(xcounter == 15 || blocks_[(counter % 16) + 1].block_type_ == 0)
					block.faces[1] = true;
				
				//Top Face
				if(ycounter >= 254 || blocks_[counter + (16 * 16)].block_type_ == 0)
					block.faces[3] = true;
				
				//Bottom Face
				if (ycounter == 0 || blocks_[counter - (16 * 16)].block_type_ == 0)
					block.faces[2] = true;
				
				//Front Face
				if(zcounter == 0 || blocks_[counter - 16].block_type_ == 0)
					block.faces[5] = true;
			
				//Back Face
				if (zcounter >= 15 || blocks_[counter + 16].block_type_ == 0)
					block.faces[4] = true;
				

				auto blockVerts = block.regenerate_model();
				
				if (!blockVerts.empty())
				{
					vertices.insert(vertices.end(), blockVerts.begin(), blockVerts.end());
				}

				if (xcounter == 15)
				{
					xcounter = 0;
					zcounter++;
				}
				else
				{
					xcounter++;
				}
				if (zcounter > 15)
				{
					zcounter = 0;
					ycounter++;
				}
				counter++;
				
				blockVerts.clear();
			}

			model = std::make_unique<Model>(device ,vertices);
		}
		
		TransformComponent transform_;
	
	private:
		uint32_t chunk_size_ = 16 * 16 * 255;
		std::vector<Block> blocks_;	
		std::shared_ptr<Model> model = nullptr;
		
		uint16_t id_;
		int32_t x_;
		int32_t z_;
	};
}