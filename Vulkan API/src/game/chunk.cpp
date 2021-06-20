#include "chunk.h"

#include <filesystem>

namespace Vulkan
{
	Chunk::Chunk(uint32_t id, int32_t x, int32_t z) : id_(id), x_(x), z_(z)
	{
		transform_.translation = { (x * 16) + 1, 0, (z * 16) + 1 };
	}

	void Chunk::generate()
	{
		std::vector<Block> x_blocks;
		std::vector<std::vector<Block>> z_blocks;
		
		uint16_t id = 0;
		unsigned int blocktype = 0;
		
		for (uint32_t y = 0; y < 255; y++)
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


					Block newBlock{ id, blocktype };
					x_blocks.push_back(std::move(newBlock));
				}
				z_blocks.push_back(std::move(x_blocks));
				x_blocks.clear();
			}
			blocks_.push_back(std::move(z_blocks));
			z_blocks.clear();
		}
	}

	void Chunk::load_block_faces(Device& device, Chunk& Left, Chunk& Right, Chunk& Front, Chunk& Back)
	{
		std::vector<Vertex> vertices;
		std::vector<Vertex> facevertices;

		for (uint8_t y = 0; y < 255; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++)
				{
					//Left Face
					if (x == 0)
					{
						if (Left.id() == UINT32_MAX || Left.blocks_[y][z][15].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());	
					}
					else if(blocks_[y][z][x - 1].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());							

					//Right Face
					if (x == 15)
					{
						if (Right.id() == UINT32_MAX || Right.blocks_[y][z][0].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());
					}
					else if (blocks_[y][z][x + 1].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());
					
					//Top Face
					if (y == 254 || blocks_[y+1][z][x].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::TopFace.begin(), cube::TopFace.end());

					//Bottom Face
					if (y == 0 || blocks_[y-1][z][x].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());

					//Front Face
					if (z == 0)
					{
						if (Front.id() == UINT32_MAX || Front.blocks_[y][15][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
					}
					else if (blocks_[y][z-1][x].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());

					//Back Face
					if (z == 15)
					{
						if (Back.id() == UINT32_MAX || Back.blocks_[y][0][x].block_type_ == 0)
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());
					}
					else if (blocks_[y][z + 1][x].block_type_ == 0)
						facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());
					
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
				}
			}
		}
		
		model = std::make_unique<Model>(id_, device, vertices);
	}

	
}
