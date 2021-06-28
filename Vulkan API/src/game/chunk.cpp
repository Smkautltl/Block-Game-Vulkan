#include "chunk.h"

//-=-=-=-=- STD -=-=-=-=-
#include <filesystem>

namespace Vulkan
{
	Chunk::Chunk()
	{
	}

	Chunk::Chunk(int32_t x, int32_t z) : x_(x), z_(z){}

	void Chunk::generate(std::vector<std::vector<float>> blockHeights)
	{	
		uint8_t blocktype = 0;

		for (uint32_t z = 0; z < chunk_length_; z++)
		{
			for (uint32_t x = 0; x < chunk_length_; x++)
			{
				uint32_t terrianHeight = (uint32_t)(40 * blockHeights[z][x]) + 70;
				for (uint32_t y = 0; y < chunk_height_; y++, blocktype = 0)
				{	
					if (y <= (terrianHeight-7))
					{
						blocktype = 3;
					}
					else if ((terrianHeight-7) < y && y <= (terrianHeight-2))
					{
						blocktype = 2;
					}
					else if (y <= terrianHeight)
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
					
					blocks_[y][z][x] = blocktype;
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
		int nan = 0;
		
		for (uint8_t y = 0; y < chunk_height_; y++)
		{
			for (uint8_t z = 0; z < chunk_length_; z++)
			{
				for (uint8_t x = 0; x < chunk_length_; x++)
				{
					//Add faces that need to be rendered
					if(blocks_[y][z][x] != 0)
					{
						//Left Face
						if (x == 0 && (Left->empty ||Left->blocks_[y][z][15] == 0))
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());
						else if ((x - 1 < 0))
							nan = 0;
						else if (blocks_[y][z][x - 1] == 0)
							facevertices.insert(facevertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());

						//Right Face
						if (x == 15 && (Right->empty || Right->blocks_[y][z][0] == 0))
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());
						else if ((x + 1 > 15))
							nan = 0;
						else if (blocks_[y][z][x + 1] == 0)
							facevertices.insert(facevertices.end(), cube::RightFace.begin(), cube::RightFace.end());

						//Top Face
						if (y + 1 == 255)
							nan = 0;
						else if (blocks_[y + 1][z][x] == 0)
							facevertices.insert(facevertices.end(), cube::TopFace.begin(), cube::TopFace.end());

						//Bottom Face
						if (y == 0 || blocks_[y - 1][z][x] == 0)
							facevertices.insert(facevertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());

						//Front Face
						if (z == 0 && (Front->empty || Front->blocks_[y][15][x] == 0))
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
						else if ((z - 1 < 0))
							nan = 0;
						else if (blocks_[y][z - 1][x] == 0)
							facevertices.insert(facevertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());

						//Back Face
						if (z == 15 && (Back->empty || Back->blocks_[y][0][x] == 0))
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());
						else if ((z + 1 > 15))
							nan = 0;
						else if (blocks_[y][z + 1][x] == 0)
							facevertices.insert(facevertices.end(), cube::BackFace.begin(), cube::BackFace.end());

						//Move faces to correct location
						if (!facevertices.empty())
						{
							int count = 0;
							for (auto& vertex : facevertices)
							{
								
								vertex.position.x += static_cast<float>(x); //+(x_ * chunk_length_));
								vertex.position.y += static_cast<float>(y);
								vertex.position.z += static_cast<float>(z);// +(z_ * chunk_length_));

								switch (blocks_[y][z][x])
								{
								case 1:
									vertex.colour.x = 0.45f ;//+ ((float)(count % 2) / 10);
									vertex.colour.y = 0.9f  ;//+ ((float)(count % 2) / 10);
									vertex.colour.z = 0.0f  ;//+ ((float)(count % 2) / 10);
									break;
								case 2:
									vertex.colour.x = 0.6f ;//+ ((float)(count % 2) / 10);
									vertex.colour.y = 0.3f ;//+ ((float)(count % 2) / 10);
									vertex.colour.z = 0.0f ;//+ ((float)(count % 2) / 10);
									break;
								case 3:
									vertex.colour.x = 0.7f ;//+ ((float)(count % 2) / 10);
									vertex.colour.y = 0.75f;// + ((float)(count % 2) / 10);
									vertex.colour.z = 0.9f ;//+ ((float)(count % 2) / 10);
									break;
								case 4:
									vertex.colour.x = 0.f  ;//+ ((float)(count % 2) / 10);
									vertex.colour.y = 0.74f;// + ((float)(count % 2) / 10);
									vertex.colour.z = 0.9f ;//+ ((float)(count % 2) / 10);
									break;
								case 5:
									vertex.colour.x = 0.9f ;//+ ((float)(count % 2) / 10);
									vertex.colour.y = 0.84f;// + ((float)(count % 2) / 10);
									vertex.colour.z = 0.0f ;//+ ((float)(count % 2) / 10);
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

		if (model != nullptr)
		{
			model->destroy();
		}
		
		model = std::make_unique<Model>(0, device, vertices);
		ready = true;
	}//Load block faces
}