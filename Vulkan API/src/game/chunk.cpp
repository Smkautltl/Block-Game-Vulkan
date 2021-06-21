#include "chunk.h"

//-=-=-=-=- STD -=-=-=-=-
#include <filesystem>

namespace Vulkan
{
	Chunk::Chunk(uint32_t id, int32_t x, int32_t z) : id_(id), x_(x), z_(z)
	{
		blocks_ = layers(255, rows(chunk_length_, blocks(chunk_length_, Block{})));
		transform_.translation = { (x * 16) + 1, 0, (z * 16) + 1 };
	}

	void Chunk::generate()
	{
		uint16_t id = 0;
		unsigned int blocktype = 0;
		
		for (uint32_t y = 0; y < chunk_height_; y++)
		{
			for (uint32_t z = 0; z < chunk_length_; z++)
			{
				for (uint32_t x = 0; x < chunk_length_; x++, id++)
				{
					if (y < 70)
					{
						blocktype = 3;
					}
					else if (70 < y && y < 74)
					{
						blocktype = 2;
					}
					else if (y == 75)
					{
						blocktype = 1;
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
						if (y == 254 || blocks_[y + 1][z][x].block_type_ == 0)
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
					}

					//Move faces to correct location
					if (!facevertices.empty())
					{
						int count = 0;
						for (auto vertex : facevertices)
						{
							auto modifiedVert = vertex;
							modifiedVert.position.x += x;
							modifiedVert.position.y += y;
							modifiedVert.position.z += z;
							facevertices[count] = modifiedVert;
							count++;
						}

						vertices.insert(vertices.end(), facevertices.begin(), facevertices.end());
						facevertices.clear();
					}
					
				}//X
			}//Z
		}//Y
		
		model = std::make_unique<Model>(id_, device, vertices);
	}//Load block faces
}