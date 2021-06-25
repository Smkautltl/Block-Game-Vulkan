#include "TerrainGenerator.h"

#include <glm/geometric.hpp>

Vulkan::TerrainGenerator::TerrainGenerator()
{
	seed = rand();
}

glm::vec2 Vulkan::TerrainGenerator::random_gradient(int ix, int iy)
{
	float random = 2920.f * sin(ix * 21942.f + iy * seed + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
	return  { cos(random), sin(random) };
}

std::vector<std::vector<float>> Vulkan::TerrainGenerator::getnoise(float x, float z)
{
	glm::vec2 ChunkGrad(-5,-5), ChunkGradX(-5, -5), ChunkGradZ(-5, -5), ChunkGradXZ(-5, -5);
	
	for (auto chunk_border : RandomChunkBorderGradients)
	{
		if (ChunkGrad != glm::vec2(-5,-5) && ChunkGradX != glm::vec2(-5, -5) && ChunkGradZ != glm::vec2(-5, -5) && ChunkGradXZ != glm::vec2(-5, -5))
		{
			break;
		}
		
		if (chunk_border.first == glm::vec2(x,z))
		{
			ChunkGrad = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(x+1, z))
		{
			ChunkGradX = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(x, z + 1))
		{
			ChunkGradZ = chunk_border.second;
			continue;
		}
		if (chunk_border.first == glm::vec2(x + 1, z + 1))
		{
			ChunkGradXZ = chunk_border.second;
		}
	}

	if (ChunkGrad == glm::vec2(-5, -5))
	{
		ChunkGrad = random_gradient(x,z);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ x,z }, ChunkGrad));
	}
	if (ChunkGradX == glm::vec2(-5, -5))
	{
		ChunkGradX = random_gradient(x+1, z);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ x+1,z }, ChunkGradX));
	}
	if (ChunkGradZ == glm::vec2(-5, -5))
	{
		ChunkGradZ = random_gradient(x, z+1);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ x,z+1 }, ChunkGradZ));
	}
	if (ChunkGradXZ == glm::vec2(-5, -5))
	{
		ChunkGradXZ = random_gradient(x+1, z+1);
		RandomChunkBorderGradients.push_back(std::make_pair(glm::vec2{ x+1,z+1 }, ChunkGradXZ));
	}

	std::vector<std::vector<float>> blockHeights(16, std::vector(16, 0.0f));

	for (auto blockZ = 0; blockZ < 16; blockZ++)
	{
		for (auto blockX = 0; blockX < 16; blockX++)
		{
			float xPercent = ((float)(blockX + 1) / 16.f);
			float zPercent = ((float)(blockZ + 1) / 16.f);
			
			glm::vec2 dist = { xPercent, zPercent};
			glm::vec2 distX = { -(1-xPercent), zPercent };
			glm::vec2 distZ = {xPercent, -(1-zPercent)};
			glm::vec2 distXZ = {-(1-xPercent), -(1-zPercent)};

			float lerp1 = interpolate(glm::dot(dist, ChunkGrad), glm::dot(distX, ChunkGradX), xPercent);
			float lerp2 = interpolate(glm::dot(distZ, ChunkGradZ), glm::dot(distXZ, ChunkGradXZ), xPercent);

			blockHeights[blockZ][blockX] = interpolate(lerp1, lerp2, zPercent);
		}
	}

	return blockHeights; //TODO: Use more than 1 chunk in the generation to get rid of swamp-like look of the terrain
}

float Vulkan::TerrainGenerator::interpolate(float a0, float a1, float w)
{
	if (-1.0f > w) return a0;
	if (1.0f < w) return a1;
	return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}
