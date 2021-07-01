#include "chunk.h"

//-=-=-=-=- STD -=-=-=-=-
#include <filesystem>

namespace Vulkan
{
	Chunk::Chunk()
	{
		x_ = 0;
		z_ = 0;
	}

	Chunk::Chunk(int32_t x, int32_t z) : x_(x), z_(z){}

	void Chunk::generate(std::vector<std::vector<float>> blockHeights)
	{	
		uint8_t blocktype = 0;
		uint8_t terrainHeight;
		for (uint32_t y = 0; y < chunk_height_; y++)
		{
			for (uint32_t z = 0; z < chunk_length_; z++)
			{
				for (uint32_t x = 0; x < chunk_length_; x++, blocktype = 0)
				{
					terrainHeight = (uint8_t)(40 * blockHeights[z][x]) + 70;
					if (y <= (terrainHeight -7))
					{
						blocktype = 3;
					}
					else if ((terrainHeight -7) < y && y <= (terrainHeight -2))
					{
						blocktype = 2;
					}
					else if (y <= terrainHeight)
					{
						blocktype = 1;
					}

					if(y < 60 && blocktype == 0)
					{
						blocktype = 4;
					}
					if (y <= 60 && blocktype == 1)
					{
						blocktype = 5;
					}

					blocks_[x + chunk_length_ * (y + chunk_height_ * z)] = blocktype;
				}
			}
		}
	}

	void Chunk::load_block_faces(Device& device, Chunk* Left, Chunk* Right, Chunk* Front, Chunk* Back)
	{
		ready = false;
		if (blocks_.empty())
		{
			return;
		}
		
		std::vector<Vertex> vertices;
		std::vector<Vertex> facevertices;
		uint8_t nan;
		
		for (uint8_t y = 0; y < chunk_height_; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++)
				{
					//Add faces that need to be rendered
					if(blocks_[x + chunk_length_ * (y + chunk_height_ * z)] != 0)
					{
					//-=-=-=-=- Left Face -=-=-=-=-
						bool Condition1 = x == 0;
						bool Condition2 = Left->empty || Left->blocks_[15 + chunk_length_ * (y + chunk_height_ * z)] == 0;
						bool Condition3 = false;
						if (!(x - 1 < 0))
							Condition3 = blocks_[(x - 1) + chunk_length_ * (y + chunk_height_ * z)] == 0;
						
						if ((Condition1 && Condition2) || Condition3)
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());
						
					//-=-=-=-=- Right Face -=-=-=-=
						Condition1 = x == 15;
						Condition2 = Right->empty || Right->blocks_[0 + chunk_length_ * (y + chunk_height_ * z)] == 0;
						Condition3 = false;
						if (!(x + 1 > 15))
							Condition3 = blocks_[(x + 1) + chunk_length_ * (y + chunk_height_ * z)] == 0;
						
						if ((Condition1 && Condition2) || Condition3)
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());

					//-=-=-=-=- Top Face -=-=-=-=-
						if (!(y + 1 == chunk_height_))
							if(blocks_[x + chunk_length_ * ((y + 1) + chunk_height_ * z)] == 0)
								facevertices.insert(facevertices.end(), cube::TopFace.begin(), cube::TopFace.end());	
						
					//-=-=-=-=- Bottom Face -=-=-=-=-
						if (y == 0 || blocks_[x + chunk_length_ * ((y-1) + chunk_height_ * z)] == 0)
							facevertices.insert(facevertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());
						
					//-=-=-=-=- Front Face -=-=-=-=-
						Condition1 = z == 0;
						Condition2 = Front->empty || Front->blocks_[x + chunk_length_ * (y + chunk_height_ * 15)] == 0;
						Condition3 = false;
						if (!(z - 1 < 0))
							Condition3 = blocks_[x + chunk_length_ * (y + chunk_height_ * (z - 1))] == 0;
						
						if ((Condition1 && Condition2) || Condition3)
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
					
					//-=-=-=-=- Back Face -=-=-=-=-=-
						Condition1 = z == 15;
						Condition2 = Back->empty || Back->blocks_[x + chunk_length_ * (y + chunk_height_ * 0)] == 0;
						Condition3 = false;
						if (!(z + 1 > 15))
							Condition3 = blocks_[x + chunk_length_ * (y + chunk_height_ * (z + 1))] == 0;
						
						if ((Condition1 && Condition2) || Condition3)
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());
						
						//Move faces to correct location within the chunk
						if (!facevertices.empty())
						{
							for (auto& vertex : facevertices)
							{
								vertex.position.x += x;
								vertex.position.y += y;
								vertex.position.z += z;
								vertices.push_back(vertex);
							}
							facevertices.clear();
						}		
					}//Is block Air?
				}//X
			}//Z
		}//Y

		if (model != nullptr)
		{
			model->destroy();
		}

		model = std::make_unique<Model>(0, device, vertices);
		ready = true;
	}//Load block faces
}