#pragma once
#include "Core/Game Components/GameObject.h"
#include "Core/Game Components/Camera.h"

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) 
{
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

#define MAX_LIGHTS 10

struct PointLight
{
	glm::vec4 pos{};
	glm::vec4 colour{};
};

struct UniformBufferObject
{
	glm::mat4 Projection{1.f};
	glm::mat4 View{1.f};
	glm::mat4 inverseView{1.f};
	glm::vec4 AmbientLightColour{1.f, 1.f, 1.f, 0.05f};
	PointLight PointLights[MAX_LIGHTS];
	int NumLights;
};

struct ShadowBufferObject
{
	std::array<glm::mat4, 4> cascadeViewProjMat;
};

struct FrameInfo
{
	FrameInfo(GameObject::Map& gameObjects, std::vector<std::unique_ptr<Texture>>& textures) : GameObjects(gameObjects), Textures(textures) {}

	int FrameIndex;
	float FrameTime;
	bool MakeFullscreen = false;
	bool MakeWindowed = false;
	bool isinWindowed = true;
	float aspect;

	glm::vec3 SunPosition{0, 0, 0};

	VkCommandBuffer CommandBuffer;

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 InverseView;
	glm::vec3 CameraPosition;

    VkDescriptorSet GlobalDescriptorSet;

	std::unique_ptr<Sampler> Sampler;
	std::vector<std::unique_ptr<Texture>>& Textures;
    GameObject::Map &GameObjects;
};