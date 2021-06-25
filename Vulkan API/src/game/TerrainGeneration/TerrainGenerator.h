#pragma once
#include <vector>
#include <glm/vec2.hpp>

#include "../../core/Log.h"

namespace Vulkan
{
	
	class TerrainGenerator
	{

	public:
		TerrainGenerator();
		std::vector<std::vector<float>> getnoise(float x, float z);

	private:
		glm::vec2 random_gradient(int ix, int iy);
		float interpolate(float a0, float a1, float w);
		
		std::vector<std::pair<glm::vec2, glm::vec2>> RandomChunkBorderGradients;
		int seed;
	};
}
