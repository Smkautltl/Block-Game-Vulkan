#pragma once
#include "../core/model.h"

namespace Vulkan
{
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
		inline static std::vector<Vertex> FrontFace
		{
			{ {-1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ {-1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ {-1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f, -1.f, 1.f},	{0.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, 1.f},	{0.0f, 1.0f, 1.0f} }
		};

		//White
		inline static std::vector<Vertex> BackFace
		{
			{ {-1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ {-1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ {-1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f, -1.f, -1.f},	{1.0f, 1.0f, 1.0f} },
			{ { 1.f,  1.f, -1.f},	{1.0f, 1.0f, 1.0f} }
		};

	};
	
	class Block
	{
	public:
		Block();
		Block(uint32_t id, uint32_t blockType, glm::vec3 Chnkpos) : chunk_position_(Chnkpos), id_(id), block_type_(blockType){}

		std::vector<Vertex> regenerate_model()
		{			
			std::vector<Vertex> vertices;
		
			if (faces.at(0) == false && faces.at(1) == false && faces.at(2) == false && faces.at(3) == false && faces.at(4) == false && faces.at(5) == false)
				return vertices;

			if (faces.at(0)== true)
				vertices.insert(vertices.end(), cube::LeftFace.begin(), cube::LeftFace.end());				

			if (faces.at(1) == true)
				vertices.insert(vertices.end(), cube::RightFace.begin(), cube::RightFace.end());			

			if (faces.at(2) == true)
				vertices.insert(vertices.end(), cube::TopFace.begin(), cube::TopFace.end());			

			if (faces.at(3) == true)
				vertices.insert(vertices.end(), cube::BottomFace.begin(), cube::BottomFace.end());	
			
			if (faces.at(4) == true)
				vertices.insert(vertices.end(), cube::FrontFace.begin(), cube::FrontFace.end());		
			
			if (faces.at(5) == true)
				vertices.insert(vertices.end(), cube::BackFace.begin(), cube::BackFace.end());		

			int count = 0;
			for (auto vertex : vertices)
			{
				auto modifiedVert = vertex;
				modifiedVert.position.x += chunk_position_.x;
				modifiedVert.position.y += chunk_position_.y;
				modifiedVert.position.z += chunk_position_.z;
				vertices[count] = modifiedVert;
				count++;
			}
			
			return vertices;
		}
		
		uint32_t block_type_;
		std::vector<bool> faces{ false, false, false, false, false, false };
	
	private:

		glm::vec3 chunk_position_;
		uint32_t id_;
		
	};
}
