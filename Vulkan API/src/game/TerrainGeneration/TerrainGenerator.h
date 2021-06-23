#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace Vulkan
{
	typedef std::vector<float> ChunkNoise;
	typedef std::vector<std::pair<glm::vec2, ChunkNoise>> WorldNoise;
	
	class TerrainGenerator
	{

	public:
		TerrainGenerator();

		ChunkNoise get_noise(int x, int z);
		
	private:
		void generate_new_region(int x, int z, int width, int height, int octaves, float bias);
		void generate_noise(int& x, int& z, int& width, int& height, ChunkNoise& seed, int octaves, float bias);
		void regenerate_seed(int x, int z);
		
		WorldNoise noise_2d_;
		ChunkNoise old_seed_;
	};
}
