#include "chunk.h"

namespace Vulkan
{
	Chunk::Chunk() : x_(0), z_(0){}
	Chunk::Chunk(int32_t x, int32_t z) : x_(x), z_(z){}

	void Chunk::generate(std::vector<float> blockHeights)
	{
		uint8_t blockType = 0;
		uint8_t terrainHeight = 0;
		for (uint8_t y = 0; y < chunk_height_; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++, blockType = 0)
				{
					terrainHeight = (uint8_t)(40 * blockHeights[z * 16 + x]) + 70;
					if (y <= (terrainHeight -7))
					{
						blockType = BlockType::Stone;
					}
					else if ((terrainHeight -7) < y && y <= (terrainHeight -2))
					{
						blockType = BlockType::Dirt;
					}
					else if (y <= terrainHeight)
					{
						blockType = BlockType::Grass;
					}

					if(y < 60 && blockType == 0)
					{
						blockType = BlockType::Water;
					}
					
					if((y <= 60 && blockType == BlockType::Grass) || (y <= 60 && blockType == BlockType::Dirt))
					{
						blockType = BlockType::Sand;
					}

					blocks_[x + chunk_length_ * (y + chunk_height_ * z)] = blockType;
				}
			}
		}
	}

	void Chunk::load_block_faces(Device& device, Chunk* Left, Chunk* Right, Chunk* Front, Chunk* Back)
	{
		ready = false;

		std::vector<Vertex> vertices;
		std::vector<Vertex> faceVertices;
		
		for (uint8_t y = 0; y < chunk_height_; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++)
				{
					if(blocks_[x + chunk_length_ * (y + chunk_height_ * z)] != BlockType::Air)
					{
						//Add faces that need to be rendered
						bool Condition1;
						bool Condition2;
						bool Condition3;

						//-=-=-=-=- Left Face -=-=-=-=-
						{
							Condition1 = x == 0;
							Condition2 = Left->empty || Left->blocks_[15 + chunk_length_ * (y + chunk_height_ * z)] == BlockType::Air;
							Condition3 = false;
							
							if (!(x - 1 < 0))
								Condition3 = blocks_[(x - 1) + chunk_length_ * (y + chunk_height_ * z)] == BlockType::Air;

							if ((Condition1 && Condition2) || Condition3)
								faceVertices.insert(faceVertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());
						}
						
						//-=-=-=-=- Right Face -=-=-=-=-
						{
							Condition1 = x == 15;
							Condition2 = Right->empty || Right->blocks_[0 + chunk_length_ * (y + chunk_height_ * z)] == BlockType::Air;
							Condition3 = false;
							
							if (!(x + 1 > 15))
								Condition3 = blocks_[(x + 1) + chunk_length_ * (y + chunk_height_ * z)] == BlockType::Air;

							if ((Condition1 && Condition2) || Condition3)
								faceVertices.insert(faceVertices.end(), cube::RightFace.begin(), cube::RightFace.end());
						}

						//-=-=-=-=- Top Face -=-=-=-=-
						{
							if (!(y + 1 == chunk_height_))
								if (blocks_[x + chunk_length_ * ((y + 1) + chunk_height_ * z)] == BlockType::Air)
									faceVertices.insert(faceVertices.end(), cube::TopFace.begin(), cube::TopFace.end());
						}

						//-=-=-=-=- Bottom Face -=-=-=-=-
						{
							if (y == 0 || blocks_[x + chunk_length_ * ((y - 1) + chunk_height_ * z)] == BlockType::Air)
								faceVertices.insert(faceVertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());
						}

						//-=-=-=-=- Front Face -=-=-=-=-
						{
							Condition1 = z == 0;
							Condition2 = Front->empty || Front->blocks_[x + chunk_length_ * (y + chunk_height_ * 15)] == BlockType::Air;
							Condition3 = false;
							
							if (!(z - 1 < 0))
								Condition3 = blocks_[x + chunk_length_ * (y + chunk_height_ * (z - 1))] == BlockType::Air;

							if ((Condition1 && Condition2) || Condition3)
								faceVertices.insert(faceVertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
						}

						//-=-=-=-=- Back Face -=-=-=-=-
						{
							Condition1 = z == 15;
							Condition2 = Back->empty || Back->blocks_[x + chunk_length_ * (y + chunk_height_ * 0)] == BlockType::Air;
							Condition3 = false;
							
							if (!(z + 1 > 15))
								Condition3 = blocks_[x + chunk_length_ * (y + chunk_height_ * (z + 1))] == BlockType::Air;

							if ((Condition1 && Condition2) || Condition3)
								faceVertices.insert(faceVertices.end(), cube::BackFace.begin(), cube::BackFace.end());
						}
						
						//Move faces to correct location within the chunk
						if (!faceVertices.empty())
						{
							for (auto& vertex : faceVertices)
							{
								vertex.position.x += x;
								vertex.position.y += y;
								vertex.position.z += z;
								
								switch (blocks_[x + chunk_length_ * (y + chunk_height_ * z)])
								{
								case BlockType::Dirt:
									vertex.colour = glm::vec3(0.4f, 0.27f, 0.0f);
									break;
									
								case BlockType::Stone:
									vertex.colour = glm::vec3(0.25f);
									break;
									
								case BlockType::Grass:
									vertex.colour = glm::vec3(0.13f, 0.4f, 0.0f);
									break;
									
								case BlockType::Water:
									vertex.colour = glm::vec3(0.0f, 0.33f, 0.4f);
									break;
									
								case BlockType::Sand:
									vertex.colour = glm::vec3(0.8f, 0.8f, 0.0f);
									break;
								default: break;
								}
								
								vertices.emplace_back(vertex);
							}
							faceVertices.clear();
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