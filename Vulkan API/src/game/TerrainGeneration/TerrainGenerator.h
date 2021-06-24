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

		glm::vec2 random_gradient(int ix, int iy);
		float getnoise(float x, float z);
	
	private:

		float dotGradient(int ix, int iy, float x, float y);
		float interpolate(float a0, float a1, float w);
		
		
	};
}
