#pragma once
//-=-=-=-=-=- STD -=-=-=-=-=-
#include <bitset>
#include <cstdint>
#include <queue>
#include <array>

//-=-=-=-=-=- GLFW -=-=-=-=-=-
#include <glm/vec3.hpp>
//#include <glm/qualifier.hpp>

namespace Vulkan
{
	using Entity = uint32_t;
	const Entity MAX_ENTITIES = 5000;

	struct Transform
	{
		glm::vec3 position;
		//glm::qua<float> rotation;
		glm::vec3 scale;
	};

	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;

	using Signature = std::bitset<MAX_COMPONENTS>;

	class EntityManager
	{
	public:
		EntityManager();

		Entity create_entity();
		void destroy_entity(Entity entity);

		void set_signature(Entity entity, Signature signature);
		Signature get_signature(Entity entity);

	private:
		std::queue<Entity> available_entities_{};
		std::array<Signature, MAX_ENTITIES> signatures_{};

		uint32_t living_entity_count_{};
	};
}
