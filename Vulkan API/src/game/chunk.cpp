#include "chunk.h"

//-=-=-=-=- STD -=-=-=-=-
#include <filesystem>

namespace Vulkan
{
	Chunk::Chunk() {}
	Chunk::Chunk(uint32_t id, int32_t x, int32_t z) : id_(id), x_(x), z_(z){}

	void Chunk::generate(TerrainGenerator& generator)
	{	
		uint16_t id = 0;
		uint8_t blocktype = 0;

		srand(x_ * z_ + z_);
		for (uint32_t z = 0; z < chunk_length_; z++)
		{
			for (uint32_t x = 0; x < chunk_length_; x++)
			{
				uint32_t terrianHeight = (10 * ((float)rand()/RAND_MAX)) + 50;
				for (uint32_t y = 0; y < chunk_height_; y++, id++, blocktype = 0)
				{	
					if (y < terrianHeight-7)
					{
						blocktype = 3;
					}
					else if (terrianHeight-7 <= y && y <= terrianHeight-3)
					{
						blocktype = 2;
					}
					else if (y <= terrianHeight)
					{
						blocktype = 1;
					}

					if(y < 60 && (blocktype == 0 || blocktype == 1))
					{
						blocktype = 4;
					}
					
					blocks_[y][z][x] = Block{ id, blocktype };
				}
			}
		}
	}

	void Chunk::load_block_faces(Device& device, Chunk* Left, Chunk* Right, Chunk* Front, Chunk* Back)
	{
		std::vector<Vertex> vertices;
		std::vector<Vertex> facevertices;
		int nan = 0;
		
		for (uint8_t y = 0; y < chunk_height_; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++)
				{
					//Add faces that need to be rendered
					if(blocks_[y][z][x].block_type_ != 0)
					{
						//Left Face
						if (x == 0 && (Left->id() == UINT32_MAX || Left->blocks_[y][z][15].block_type_ == 0))
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());
						else if (x - 1 < 0)
							nan = 0;
						else if (blocks_[y][z][x - 1].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());

						//Right Face
						if (x == 15 && (Right->id() == UINT32_MAX || Right->blocks_[y][z][0].block_type_ == 0))
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());
						else if (x + 1 > 15)
							nan = 0;
						else if (blocks_[y][z][x + 1].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());

						//Top Face
						if (y + 1 == 255)
							nan = 0;
						else if (blocks_[y + 1][z][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::TopFace.begin(), cube::TopFace.end());

						//Bottom Face
						if (y == 0 || blocks_[y - 1][z][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());

						//Front Face
						if ((z == 0 && (Front->id() == UINT32_MAX || Front->blocks_[y][15][x].block_type_ == 0)))
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
						else if (z - 1 < 0)
							nan = 0;
						else if (blocks_[y][z - 1][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());

						//Back Face
						if (z == 15 && (Back->id() == UINT32_MAX || Back->blocks_[y][0][x].block_type_ == 0))
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());
						else if (z + 1 > 15)
							nan = 0;
						else if (blocks_[y][z + 1][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());

						//Move faces to correct location
						if (!facevertices.empty())
						{
							int count = 0;
							for (auto& vertex : facevertices)
							{
								vertex.position.x += static_cast<float>(x + (x_ * chunk_length_));
								vertex.position.y += static_cast<float>(y);
								vertex.position.z += static_cast<float>(z + (z_ * chunk_length_));

								switch (blocks_[y][z][x].block_type_)
								{
								case 1:
									vertex.colour.x = 0.45f;
									vertex.colour.y = 0.9f;
									vertex.colour.z = 0.0f;
									break;
								case 2:
									vertex.colour.x = 0.6f;
									vertex.colour.y = 0.3f;
									vertex.colour.z = 0.0f;
									break;
								case 3:
									vertex.colour.x = 0.7f;
									vertex.colour.y = 0.75f;
									vertex.colour.z = 1.0f;
									break;
								case 4:
									vertex.colour.x = 0.f;
									vertex.colour.y = 0.84f;
									vertex.colour.z = 1.0f;
									break;
								}
								
								vertices.push_back(vertex);
								count++;
								
							}

							facevertices.clear();
						}
						
					}//Is block Air?
				}//X
			}//Z
		}//Y
		
		model = std::make_unique<Model>(id_, device, vertices);
	}//Load block faces
}