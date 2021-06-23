#include "TerrainGenerator.h"

#include <cstdlib>

Vulkan::TerrainGenerator::TerrainGenerator(){}

Vulkan::ChunkNoise Vulkan::TerrainGenerator::get_noise(int x, int z)
{
	int count = 0;
	for (auto noise : noise_2d_)
	{
		ChunkNoise value;
		if (noise.first == glm::vec2{z,x})
		{
			value = std::move(noise.second);
			noise_2d_.erase(noise_2d_.begin() + count);
			return value;
		}
		count++;
	}

	generate_new_region(x, z, 128, 128, 6, 2.f);

	count = 0;
	for (auto noise : noise_2d_)
	{
		ChunkNoise value;
		if (noise.first == glm::vec2{ z,x })
		{
			value = std::move(noise.second);
			noise_2d_.erase(noise_2d_.begin() + count);
			return value;
		}
		count++;
	}
}

void Vulkan::TerrainGenerator::generate_new_region(int x, int z, int width, int height, int octaves, float bias)
{
	srand(x * z + z);

	ChunkNoise noise_seed_(width * height);
	for (auto i = 0; i < width * height; i++)
		noise_seed_[i] = (float)rand() / (float)RAND_MAX;

	generate_noise(x, z, width, height, noise_seed_, octaves, bias);
	old_seed_ = noise_seed_;
}

void Vulkan::TerrainGenerator::generate_noise(int& x, int& z, int& width, int& height, ChunkNoise& seed, int octaves, float bias)
{
	std::vector<std::vector<ChunkNoise>> noise(width/16, std::vector<ChunkNoise>(height/16, ChunkNoise()));
	
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < octaves; o++)
			{
				int nPitch = width >> o;
				int nSampleX1 = (w / nPitch) * nPitch;
				int nSampleY1 = (h / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % width;
				int nSampleY2 = (nSampleY1 + nPitch) % width;

				float fBlendX = (float)(w - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(h - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * seed[nSampleY1 * width + nSampleX1] + fBlendX * seed[nSampleY1 * width + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * seed[nSampleY2 * width + nSampleX1] + fBlendX * seed[nSampleY2 * width + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / bias;
			}

			// Scale to seed range
			noise[w/16][h/16].push_back(fNoise / fScaleAcc);
		}
	}

	for (int z_ = 0; z_ < 4; z_++)
	{
		for (int x_ = 0; x_ < 4; x_++)
		{
			noise_2d_.push_back(std::make_pair(glm::vec2(z+z_, x+x_), noise[z_][x_]));
		}
	}
	
	
}

void Vulkan::TerrainGenerator::regenerate_seed(int x, int z)
{
	//srand(x * z);
	//
	//for (auto i = 0; i < width_ * height_; i++)
	//	noise_seed_2d_[i] = (float)rand() / (float)RAND_MAX;
	//
}