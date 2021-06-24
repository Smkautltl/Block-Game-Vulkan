#pragma once
#include <vector>
#include <glm/vec2.hpp>

#include "../../core/Log.h"

namespace Vulkan
{
	typedef std::vector<float> ChunkNoise;
	typedef std::vector<std::pair<glm::vec2, ChunkNoise>> WorldNoise;

	struct OldSeed
	{
		ChunkNoise seed_;
		glm::vec2 location;

		bool LeftUsed, RightUsed, TopUsed, BottomUsed;
		
		void set(ChunkNoise Seed, int x, int z) { seed_ = Seed; location.x = x; location.y = z; }
		ChunkNoise get() const { return seed_; }
		glm::vec2 get_location() const { return location; }

		bool isFullyUsed() { return (LeftUsed && RightUsed && TopUsed && BottomUsed); }
		
		ChunkNoise Left()
		{
			ChunkNoise left;
			for (auto i = 0; i < 64; i++)
			{
				left.push_back(seed_[i]);
			}
			LeftUsed = true;
			return left;
		}
		ChunkNoise Right()
		{
			ChunkNoise right;
			for (auto i = seed_.size()-1; i > seed_.size()-64; i--)
			{
				right.push_back(seed_[i]);
			}
			RightUsed = true;
			return right;
		}
		ChunkNoise Top()
		{
			ChunkNoise top;
			for (auto i = 63; i < seed_.size(); i+=64)
			{
				top.push_back(seed_[i]);
			}
			TopUsed = true;
			return top;
		}
		ChunkNoise Bottom()
		{
			ChunkNoise bottom;
			for (auto i = 0; i < seed_.size(); i += 64)
			{
				bottom.push_back(seed_[i]);
			}
			BottomUsed = true;
			return bottom;
		};
	};
	
	class TerrainGenerator
	{

	public:
		TerrainGenerator();

		ChunkNoise get_noise(int x, int z);

		glm::vec2 random_gradient(int ix, int iy);
		//Takes in the x and z that have been divided by a value to make a float
		float getnoise(float x, float z);
	
	private:
		void generate_new_region(int x, int z, int width, int height, int octaves, float bias);
		void generate_noise(int& x, int& z, int& width, int& height, ChunkNoise& seed, int octaves, float bias);
		void regenerate_seed(int x, int z);

		float dotGradient(int ix, int iy, float x, float y);
		float interpolate(float a0, float a1, float w);
		
		
		WorldNoise noise_2d_;
		std::vector<OldSeed> old_seeds_;
	};
}
