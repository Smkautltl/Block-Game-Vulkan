#include "TerrainGenerator.h"

#include <cstdlib>

Vulkan::TerrainGenerator::TerrainGenerator(){}

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
