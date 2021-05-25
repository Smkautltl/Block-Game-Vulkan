#include "entityManager.h"
#include "../core/Log.h"

namespace Vulkan
{
	EntityManager::EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			available_entities_.push(entity);
		}
	}

	Entity EntityManager::create_entity()
	{
		VK_ASSERT(living_entity_count_ < MAX_ENTITIES, "Too many entities have been made!")

		Entity id = available_entities_.front();
		available_entities_.pop();
		++living_entity_count_;

		return id;
	}

	void EntityManager::destroy_entity(Entity entity)
	{
		VK_ASSERT(entity < MAX_ENTITIES, "Entity out of range!")

		signatures_[entity].reset();

		available_entities_.push(entity);
		--living_entity_count_;
	}

	void EntityManager::set_signature(Entity entity, Signature signature)
	{
		VK_ASSERT(entity < MAX_ENTITIES, "Entity out of range!")
		signatures_[entity] = signature;
	}

	Signature EntityManager::get_signature(Entity entity)
	{
		VK_ASSERT(entity < MAX_ENTITIES, "Entity out of range!")
		return signatures_[entity];
	}
}
