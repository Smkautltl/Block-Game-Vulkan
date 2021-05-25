#pragma once

//-=-=-=-=- CORE -=-=-=-=-=-
#include "model.h"

//-=-=-=-=- STD -=-=-=-=-=-
#include <memory>

namespace Vulkan
{
	struct Transform2DComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{1.f, 1.f};
		float rotation;
		
		glm::mat2 mat2()
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{{c,s}, {-s, c}};
			
			glm::mat2 scaleMat{ { scale.x, .0f }, { .0f, scale.y } };
			return rotMatrix * scaleMat;
		}
	};
	
	class GameObject
	{
	//-=-=-=-=- Variables -=-=-=-=-=-
	public:
		using id_t = unsigned int;
		
		std::shared_ptr<Model> model{};
		glm::vec3 colour{};

		Transform2DComponent transform_2d{};
	private:
		id_t id;

	//-=-=-=-=- Methods -=-=-=-=-=-
	public:
		static GameObject createGameObject()
		{
			static id_t currentId = 0;
			return GameObject(currentId++);
		}
		const id_t getID() { return id; }

		GameObject(const GameObject &) = delete;
		GameObject &operator=(const GameObject &) = delete;
		GameObject(GameObject &&) = default;
		GameObject &operator=(GameObject &&) = default;
	
	private:
		GameObject(id_t objId) : id(objId) {}
		
	};
}