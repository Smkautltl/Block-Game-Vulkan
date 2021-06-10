#pragma once
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
		inline static std::vector<Model::Vertex> LeftFace
		{
			{ {-.5f, -.5f, -.5f},	{1.0f, 0.0f, 0.0f} },
			{ {-.5f,  .5f,  .5f},	{1.0f, 0.0f, 0.0f} },
			{ {-.5f, -.5f,  .5f},	{1.0f, 0.0f, 0.0f} },
			{ {-.5f, -.5f, -.5f},	{1.0f, 0.0f, 0.0f} },
			{ {-.5f,  .5f, -.5f},	{1.0f, 0.0f, 0.0f} },
			{ {-.5f,  .5f,  .5f},	{1.0f, 0.0f, 0.0f} }
		};

		//Green
		inline static std::vector<Model::Vertex> RightFace
		{
			{ {.5f, -.5f, -.5f},		{0.0f, 1.0f, 0.0f} },
			{ {.5f,  .5f,  .5f},		{0.0f, 1.0f, 0.0f} },
			{ {.5f, -.5f,  .5f},		{0.0f, 1.0f, 0.0f} },
			{ {.5f, -.5f, -.5f},		{0.0f, 1.0f, 0.0f} },
			{ {.5f,  .5f, -.5f},		{0.0f, 1.0f, 0.0f} },
			{ {.5f,  .5f,  .5f},		{0.0f, 1.0f, 0.0f} }
		};

		//Blue
		inline static std::vector<Model::Vertex> TopFace
		{
			{ {-.5f, -.5f, -.5f},	{0.0f, 0.0f, 1.0f} },
			{ { .5f, -.5f,  .5f},	{0.0f, 0.0f, 1.0f} },
			{ {-.5f, -.5f,  .5f},	{0.0f, 0.0f, 1.0f} },
			{ {-.5f, -.5f, -.5f},	{0.0f, 0.0f, 1.0f} },
			{ { .5f, -.5f, -.5f},	{0.0f, 0.0f, 1.0f} },
			{ { .5f, -.5f,  .5f},	{0.0f, 0.0f, 1.0f} }
		};

		//Yellow
		inline static std::vector<Model::Vertex> BottomFace
		{
			{ {-.5f, .5f, -.5f},		{1.0f, 1.0f, 0.0f} },
			{ { .5f, .5f,  .5f},		{1.0f, 1.0f, 0.0f} },
			{ {-.5f, .5f,  .5f},		{1.0f, 1.0f, 0.0f} },
			{ {-.5f, .5f, -.5f},		{1.0f, 1.0f, 0.0f} },
			{ { .5f, .5f, -.5f},		{1.0f, 1.0f, 0.0f} },
			{ { .5f, .5f,  .5f},		{1.0f, 1.0f, 0.0f} }
		};

		//Turquoise
		inline static std::vector<Model::Vertex> FrontFace
		{
			{ {-.5f, -.5f, 0.5f},	{0.0f, 1.0f, 1.0f} },
			{ { .5f,  .5f, 0.5f},	{0.0f, 1.0f, 1.0f} },
			{ {-.5f,  .5f, 0.5f},	{0.0f, 1.0f, 1.0f} },
			{ {-.5f, -.5f, 0.5f},	{0.0f, 1.0f, 1.0f} },
			{ { .5f, -.5f, 0.5f},	{0.0f, 1.0f, 1.0f} },
			{ { .5f,  .5f, 0.5f},	{0.0f, 1.0f, 1.0f} }
		};

		//White
		inline static std::vector<Model::Vertex> BackFace
		{
			{ {-.5f, -.5f, -0.5f},	{1.0f, 1.0f, 1.0f} },
			{ { .5f,  .5f, -0.5f},	{1.0f, 1.0f, 1.0f} },
			{ {-.5f,  .5f, -0.5f},	{1.0f, 1.0f, 1.0f} },
			{ {-.5f, -.5f, -0.5f},	{1.0f, 1.0f, 1.0f} },
			{ { .5f, -.5f, -0.5f},	{1.0f, 1.0f, 1.0f} },
			{ { .5f,  .5f, -0.5f},	{1.0f, 1.0f, 1.0f} }
		};

	};
	
	class Block
	{
	public:
		Block(uint32_t id, uint32_t blockType) : id_(id), block_type_(blockType) {}

		void regenerate_model(Device& device, std::vector<bool> faces)
		{
			if (faces[0] == false && faces[1] == false && faces[2] == false && faces[3] == false && faces[4] == false && faces[5] == false)
			{
				return;
			}
			
			std::vector<Model::Vertex> vertices;

			if (faces[0] == true)
				vertices.insert(vertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());

			if (faces[1] == true)
				vertices.insert(vertices.end(), cube::RightFace.begin(), cube::RightFace.end());

			if (faces[2] == true)
				vertices.insert(vertices.end(), cube::TopFace.begin(), cube::TopFace.end());

			if (faces[3] == true)
				vertices.insert(vertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());
			
			if (faces[4] == true)
				vertices.insert(vertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());
			
			if (faces[5] == true)
				vertices.insert(vertices.end(), cube::BackFace.begin(), cube::BackFace.end());
	
			model = std::make_unique<Model>(device, vertices);
		}
		
		std::shared_ptr<Model> Get() { return model; }
		uint32_t get_block_type_() { return block_type_; }

	public:
		glm::vec3 colour{};
		TransformComponent transform_{};
	
	private:
		std::shared_ptr<Model> model;
		
		uint32_t id_;
		uint32_t block_type_;
	};
}
