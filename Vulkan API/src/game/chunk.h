#pragma once
#include "block.h"

#include "../core/model.h"

namespace Vulkan
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
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
			{ {-1.f, -1.f, -1.f},	{1.0f, 0.0f, 0.0f} },
			{ {-1.f,  1.f,  1.f},	{1.0f, 0.0f, 0.0f} },
			{ {-1.f, -1.f,  1.f},	{1.0f, 0.0f, 0.0f} },
			{ {-1.f, -1.f, -1.f},	{1.0f, 0.0f, 0.0f} },
			{ {-1.f,  1.f, -1.f},	{1.0f, 0.0f, 0.0f} },
			{ {-1.f,  1.f,  1.f},	{1.0f, 0.0f, 0.0f} }
		};

		//Green
		inline static std::vector<Vertex> RightFace
		{
			{ {1.f, -1.f, -1.f},		{0.0f, 1.0f, 0.0f} },
			{ {1.f,  1.f,  1.f},		{0.0f, 1.0f, 0.0f} },
			{ {1.f, -1.f,  1.f},		{0.0f, 1.0f, 0.0f} },
			{ {1.f, -1.f, -1.f},		{0.0f, 1.0f, 0.0f} },
			{ {1.f,  1.f, -1.f},		{0.0f, 1.0f, 0.0f} },
			{ {1.f,  1.f,  1.f},		{0.0f, 1.0f, 0.0f} }
		};

		//Blue
		inline static std::vector<Vertex> TopFace
		{
			{ {-1.f, -1.f, -1.f},	{0.0f, 0.0f, 1.0f} },
			{ { 1.f, -1.f,  1.f},	{0.0f, 0.0f, 1.0f} },
			{ {-1.f, -1.f,  1.f},	{0.0f, 0.0f, 1.0f} },
			{ {-1.f, -1.f, -1.f},	{0.0f, 0.0f, 1.0f} },
			{ { 1.f, -1.f, -1.f},	{0.0f, 0.0f, 1.0f} },
			{ { 1.f, -1.f,  1.f},	{0.0f, 0.0f, 1.0f} }
		};

		//Yellow
		inline static std::vector<Vertex> BottomFace
		{
			{ {-1.f, 1.f, -1.f},		{1.0f, 1.0f, 0.0f} },
			{ { 1.f, 1.f,  1.f},		{1.0f, 1.0f, 0.0f} },
			{ {-1.f, 1.f,  1.f},		{1.0f, 1.0f, 0.0f} },
			{ {-1.f, 1.f, -1.f},		{1.0f, 1.0f, 0.0f} },
			{ { 1.f, 1.f, -1.f},		{1.0f, 1.0f, 0.0f} },
			{ { 1.f, 1.f,  1.f},		{1.0f, 1.0f, 0.0f} }
		};

		//Turquoise
		inline static std::vector<Vertex> BackFace
		{
			{ {-1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ {-1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ {-1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} }
		};

		//White
		inline static std::vector<Vertex> FrontFace
		{
			{ {-1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ {-1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ {-1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} }
		};

	};
	
	class Chunk
	{
	public:

		Chunk(uint32_t id, int32_t x, int32_t z);
		~Chunk()
		{			
			if (model != nullptr)
			{
				VK_CORE_WARN("Chunk ID:{0} | Destructor called!", id_)
				model->destroy();
			}
			
		}

		uint32_t id() { return id_; }
		std::pair<int32_t, int32_t> xz_Coords() { return std::make_pair(x_, z_); }
		std::shared_ptr<Model> get() { return model; }
	
		void generate();
		
		void load_block_faces(Device& device, Chunk* Left, Chunk* Right, Chunk* Front, Chunk* Back);
		
		TransformComponent transform_;
	
	private:
		uint8_t chunk_length_ = 16;
		uint8_t chunk_size_ = chunk_length_;
		uint16_t chunk_volume_ = chunk_size_ * chunk_length_  * 255;
		std::vector<std::vector<std::vector<Block>>> blocks_;	
		std::shared_ptr<Model> model = nullptr;
		
		uint32_t id_;
		int32_t x_;
		int32_t z_;
	};

	
}
