#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "../core/model.h"

//-=-=-=-=- GAME -=-=-=-=-
#include "TerrainGeneration/TerrainGenerator.h"


namespace Vulkan
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 0.5f, 0.5f, 0.5f };
		glm::vec3 rotation;

		glm::mat4 mat4()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			return glm::mat4
			{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
		}
	};
	struct cube
	{
		//Red
		inline static std::vector<Vertex> LeftFace
		{
			{ {-0.5f, -0.5f, -0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} },
			{ {-0.5f,  0.5f,  0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} },
			{ {-0.5f, -0.5f,  0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} },
			{ {-0.5f, -0.5f, -0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} },
			{ {-0.5f,  0.5f, -0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} },
			{ {-0.5f,  0.5f,  0.5f},		{1.0f, 0.0f, 0.0f}, {-1.f,0,0} }
		};

		//Green
		inline static std::vector<Vertex> RightFace
		{
			{ {0.5f, -0.5f, -0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} },
			{ {0.5f,  0.5f,  0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} },
			{ {0.5f, -0.5f,  0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} },
			{ {0.5f, -0.5f, -0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} },
			{ {0.5f,  0.5f, -0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} },
			{ {0.5f,  0.5f,  0.5f},		{0.0f, 1.0f, 0.0f}, {1.f,0,0} }
		};

		//Blue
		inline static std::vector<Vertex> BottomFace
		{
			{ {-0.5f, -0.5f, -0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} },
			{ { 0.5f, -0.5f,  0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} },
			{ {-0.5f, -0.5f,  0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} },
			{ {-0.5f, -0.5f, -0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} },
			{ { 0.5f, -0.5f, -0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} },
			{ { 0.5f, -0.5f,  0.5f},		{0.0f, 0.0f, 1.0f}, {0,-1.f,0} }
		};

		//Yellow
		inline static std::vector<Vertex> TopFace
		{
			{ {-0.5f, 0.5f, -0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  },
			{ { 0.5f, 0.5f,  0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  },
			{ {-0.5f, 0.5f,  0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  },
			{ {-0.5f, 0.5f, -0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  },
			{ { 0.5f, 0.5f, -0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  },
			{ { 0.5f, 0.5f,  0.5f},		{1.0f, 1.0f, 0.0f}, {0,1.f,0}  }
		};

		//Turquoise
		inline static std::vector<Vertex> BackFace
		{
			{ {-0.5f, -0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} },
			{ { 0.5f,  0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} },
			{ {-0.5f,  0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} },
			{ {-0.5f, -0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} },
			{ { 0.5f, -0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} },
			{ { 0.5f,  0.5f, 0.5f},		{0.0f, 1.0f, 1.0f} , {0, 0, 1.f} }
		};

		//White
		inline static std::vector<Vertex> FrontFace
		{
			{ {-0.5f, -0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}},
			{ { 0.5f,  0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}},
			{ {-0.5f,  0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}},
			{ {-0.5f, -0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}},
			{ { 0.5f, -0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}},
			{ { 0.5f,  0.5f, -0.5f},		{1.0f, 1.0f, 1.0f} , {0, 0, -1.f}}
		};

	};
	
	class Chunk
	{
	public:
		
		Chunk();
		Chunk( int32_t x, int32_t z);
		~Chunk()
		{
			if (model != nullptr)
			{
				model->destroy();
			}
		}

		void update_chunkdata(int x, int z, std::vector<std::vector<float>> blockHeights)
		{
			blocks_ = std::vector((uint16_t)chunk_height_ * (chunk_length_ * chunk_length_), (uint8_t)10);
			x_ = x;
			z_ = z;
			transform_.translation = {x*16, 0, z*16};
			empty = false;
			generate(blockHeights);
		}
		bool is_model_valid() { return model != nullptr; }
		std::shared_ptr<Model> get() { return model; }
		
		void generate(std::vector<std::vector<float>> blockHeights);
		void load_block_faces(Device& device, Chunk* Left, Chunk* Right, Chunk* Front, Chunk* Back);
		
		TransformComponent transform_{glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.f) };
		bool empty = true;
		bool ready = false;
		int32_t x_;
		int32_t z_;

		std::mutex lock;

		const uint8_t chunk_length_ = 16;
		const uint8_t chunk_height_ = 255;
		//const uint16_t chunk_volume_ = chunk_length_ * chunk_length_  * chunk_height_;

		//std::vector<std::vector<std::vector<UINT8>>> blocks_;
		std::vector<uint8_t> blocks_;
		std::shared_ptr<Model> model = nullptr;
	
	private:
		
	};

	
}
