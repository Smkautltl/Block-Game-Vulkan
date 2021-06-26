#pragma once
#include <vector>
#include <glm/vec2.hpp>

#include "../../core/Log.h"

namespace Vulkan
{
	
	class TerrainGenerator
	{

	public:
		//Takes in a value for how many chunks should be used in the getNoise generation
		TerrainGenerator(int chunkScale);
		std::vector<std::vector<float>> getnoise(int x, int z);

	private:
		glm::vec2 random_gradient(int ix, int iy);
		float interpolate(float a0, float a1, float w);
		
		std::vector<std::pair<glm::vec2, glm::vec2>> RandomChunkBorderGradients;
		float seed;
		int chunk_scale_;
	};
}
