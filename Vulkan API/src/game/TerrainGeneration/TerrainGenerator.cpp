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

	generate_new_region(x, z, 66, 66, 6, 2.f);

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

glm::vec2 Vulkan::TerrainGenerator::random_gradient(int ix, int iy)
{
	float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
	return  { cos(random), sin(random) };
}

float Vulkan::TerrainGenerator::getnoise(float x, float z)
{
	int x0 = (int)x;
	int x1 = x0 + 1;
	int y0 = (int)z;
	int y1 = y0 + 1;

	float sx = x - (float)x0;
	float sy = z - (float)y0;

	float n0, n1, ix0, ix1, value;

	n0 = dotGradient(x0, y0, x, z);
	n1 = dotGradient(x1, y0, x, z);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGradient(x0, y1, x, z);
	n1 = dotGradient(x1, y1, x, z);
	ix1 = interpolate(n0, n1, sx);

	value = interpolate(ix0, ix1, sy);
	return value;
}

void Vulkan::TerrainGenerator::generate_new_region(int x, int z, int width, int height, int octaves, float bias)
{
	srand(x * z + rand());

	ChunkNoise noise_seed_(width * height);
	for (auto i = 0; i < width * height; i++)
		noise_seed_[i] = (float)rand() / (float)RAND_MAX;

	for (auto seed : old_seeds_)
	{
		auto seedLoc = seed.get_location();
		if ((int)seedLoc.x == x-4)
		{
			auto seedData = seed.Left();
			for (auto i = 0; i < 64; i++)
			{
				noise_seed_[i] = seedData[i];
			}
		}
		if ((int)seedLoc.x == x + 4)
		{
			auto seedData = seed.Right();
			int count = 0;
			for (auto i = noise_seed_.size()-1; i > noise_seed_.size() - 64; i--, count++)
			{
				noise_seed_[i] = seedData[count];
			}
		}
		if ((int)seedLoc.y == z - 4)
		{
			auto seedData = seed.Bottom();
			int count = 0;
			for (auto i = 0; i < noise_seed_.size(); i += 64, count++)
			{
				noise_seed_[i] = seedData[count];
			}
		}
		if ((int)seedLoc.y == z + 4)
		{
			auto seedData = seed.Top();
			int count = 0;
			for (auto i = 63; i < noise_seed_.size(); i += 64, count++)
			{
				noise_seed_[i] = seedData[count];
			}
		}
	}
	
	generate_noise(x, z, width, height, noise_seed_, octaves, bias);

	OldSeed oldseed{ noise_seed_, {x, z} };
	old_seeds_.push_back(oldseed);
}

void Vulkan::TerrainGenerator::generate_noise(int& x, int& z, int& width, int& height, ChunkNoise& seed, int octaves, float bias)
{
	std::vector<std::vector<ChunkNoise>> noise((width-2)/16, std::vector<ChunkNoise>((height-2)/16, ChunkNoise()));
	
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

			if ((h != 0 && w != 0) && (h <= 64 && w <= 64))
			{
				noise[(w-1) / 16][(h - 1) / 16].push_back(fNoise / fScaleAcc);
				//VK_CORE_TRACE("Noise value={0}\tplaced into {1}-{2}", (fNoise/fScaleAcc), (w-1)/16, (h-1)/16)
			}
			
		}
	}

	for (int z_ = 0; z_ < 4; z_++)
	{
		for (int x_ = 0; x_ < 4; x_++)
		{
			noise_2d_.push_back(std::make_pair(glm::vec2(z+z_, x+x_), noise[x_][z_]));
		}
	}
	
	
}

float Vulkan::TerrainGenerator::dotGradient(int ix, int iy, float x, float y)
{
	glm::vec2 gradient = random_gradient(ix, iy);

	float dx = x - (float)ix;
	float dy = y - (float)iy;

	return (dx * gradient.x) + (dy * gradient.y);
}

float Vulkan::TerrainGenerator::interpolate(float a0, float a1, float w)
{
	if (0.0f > w) return a0;
	if (1.0f < w) return a1;
	return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}
