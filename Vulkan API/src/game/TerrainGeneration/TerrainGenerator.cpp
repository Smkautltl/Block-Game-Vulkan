#include "TerrainGenerator.h"

#include <glm/geometric.hpp>

Vulkan::TerrainGenerator::TerrainGenerator(int chunkScale) : chunk_scale_(chunkScale)
{
	seed = rand();
}

glm::vec2 Vulkan::TerrainGenerator::random_gradient(int ix, int iy)
{
	float random = 2920.f * sin(ix * 21942.f + iy * seed + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
	return  { cos(random), sin(random) };
}

std::vector<std::vector<float>> Vulkan::TerrainGenerator::getnoise(int x, int z)
{
	int xMod = (x) % chunk_scale_;
	int zMod = (z) % chunk_scale_;

	if (x < 0)
	{
		xMod = (-x) % chunk_scale_;
	}
	if(z < 0)
	{
		zMod = (-z) % chunk_scale_;
	}
	
	if (xMod < 0)
	{
		xMod = -xMod;
	}
	if (zMod < 0)
	{
		zMod = -zMod;
	}
	
	int ScaledX = x - (chunk_scale_ - xMod);
	int ScaledZ = z - (chunk_scale_ - zMod);

	if (x > 0)
	{
		ScaledX = x - xMod;
	}
	if (z > 0)
	{
		ScaledZ = z - zMod;
	}
	
	if (xMod == 0)
	{
		ScaledX = x;
	}
	if (zMod == 0)
	{
		ScaledZ = z;
	}
	
	glm::vec2 ChunkGrad(-5, -5), ChunkGradX(-5, -5), ChunkGradZ(-5, -5), ChunkGradXZ(-5, -5);
	
	//TODO: Remove RandomChunkBorderGradients when they have been fully used
	for (auto chunk_border : RandomChunkBorderGradients)
	{
		if (ChunkGrad != glm::vec2(-5,-5) && ChunkGradX != glm::vec2(-5, -5) && ChunkGradZ != glm::vec2(-5, -5) && ChunkGradXZ != glm::vec2(-5, -5))
		{
			break;
		}
		
		if (chunk_border.first == glm::vec2(ScaledX, ScaledZ))
		{
			ChunkGrad = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(ScaledX + chunk_scale_, ScaledZ))
		{
			ChunkGradX = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(ScaledX, ScaledZ + chunk_scale_))
		{
			ChunkGradZ = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(ScaledX + chunk_scale_, ScaledZ + chunk_scale_))
		{
			ChunkGradXZ = chunk_border.second;
		}
	}

	if (ChunkGrad == glm::vec2(-5, -5))
	{
		ChunkGrad = random_gradient(ScaledX, ScaledZ);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ ScaledX,ScaledZ }, ChunkGrad));
	}
	if (ChunkGradX == glm::vec2(-5, -5))
	{
		ChunkGradX = random_gradient(ScaledX + chunk_scale_, ScaledZ);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ ScaledX + chunk_scale_,ScaledZ }, ChunkGradX));
	}
	if (ChunkGradZ == glm::vec2(-5, -5))
	{
		ChunkGradZ = random_gradient(ScaledX, ScaledZ + chunk_scale_);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ ScaledX,ScaledZ + chunk_scale_ }, ChunkGradZ));
	}
	if (ChunkGradXZ == glm::vec2(-5, -5))
	{
		ChunkGradXZ = random_gradient(ScaledX + chunk_scale_, ScaledZ + chunk_scale_);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ ScaledX + chunk_scale_, ScaledZ + chunk_scale_ }, ChunkGradXZ));
	}

	std::vector<std::vector<float>> blockHeights(16, std::vector(16, 0.0f));

	auto rangeX = 16 * (chunk_scale_ - xMod);
	auto rangeZ = 16 * (chunk_scale_ - zMod);

	if(x > 0)
	{
		rangeX = 16 * xMod;
	}
	if(z > 0)
	{
		rangeZ = 16 * zMod;
	}
	
	if (xMod == 0)
	{
		rangeX = 0;
	}
	if (zMod == 0)
	{
		rangeZ = 0;
	}
	
	for (auto blockZ = rangeZ; blockZ < rangeZ + 16; blockZ++)
	{
		for (auto blockX = rangeX; blockX < rangeX + 16; blockX++)
		{
			float xPercent = ((float)(blockX + 1) / (16.f*chunk_scale_));
			float zPercent = ((float)(blockZ + 1) / (16.f*chunk_scale_));
			
			glm::vec2 dist = { xPercent, zPercent};
			glm::vec2 distX = { -(1-xPercent), zPercent };
			glm::vec2 distZ = {xPercent, -(1-zPercent)};
			glm::vec2 distXZ = {-(1-xPercent), -(1-zPercent)};

			float lerp1 = interpolate(glm::dot(dist, ChunkGrad), glm::dot(distX, ChunkGradX), xPercent);
			float lerp2 = interpolate(glm::dot(distZ, ChunkGradZ), glm::dot(distXZ, ChunkGradXZ), xPercent);

			blockHeights[blockZ - rangeZ][blockX- rangeX] = interpolate(lerp1, lerp2, zPercent);
		}
	}

	return blockHeights;
}

float Vulkan::TerrainGenerator::interpolate(float a0, float a1, float w)
{
	if (-1.0f > w) return a0;
	if (1.0f < w) return a1;
	return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}
