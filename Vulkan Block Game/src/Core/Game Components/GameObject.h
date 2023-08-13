#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

#include "Model.h"
#include "Core/Vulkan/Texture.h"

struct TransformComponent
{
	glm::vec3 translation{};
	glm::vec3 scale{1.f, 1.f, 1.f};
	glm::vec3 rotation{};

	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

struct PointLightComponent
{
	float lightIntensity = 1.0f;
};

class GameObject
{
public:
	using id_t = unsigned int;
	using Map = std::unordered_map<id_t, GameObject>;

	GameObject(const GameObject&) = delete;
	GameObject &operator=(const GameObject&) = delete;
	GameObject(GameObject&&) = default;
	GameObject &operator=(GameObject&&) = default;

	static GameObject MakePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

	static GameObject CreateGameObject()
	{
		static id_t currentId = 0;
		return GameObject{ currentId++ };
	}

	id_t GetID() { return id; }

private:
	GameObject(id_t objID) : id(objID) {}

public:
	glm::vec3 colour{};
	TransformComponent transform{};

	std::shared_ptr<Model> model;
	std::shared_ptr<Texture> diffuseMap;
	std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
	id_t id;

};