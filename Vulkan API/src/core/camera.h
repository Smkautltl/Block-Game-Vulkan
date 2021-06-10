#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Vulkan
{
	class camera
	{
	public:
		void UpdateCameraPos(int key, int scancode, int action, int mods)
		{

			if (key == GLFW_KEY_W && action == GLFW_PRESS || keysUnreleased->at(GLFW_KEY_W) == true)
			{
				pos[2] += 0.1f;
				keysUnreleased->at(GLFW_KEY_W) = true;
			}
			if (key == GLFW_KEY_S && action == GLFW_PRESS || keysUnreleased->at(GLFW_KEY_S) == true)
			{
				pos[2] -= 0.1f;
				keysUnreleased->at(GLFW_KEY_S) = true;
			}
			if (key == GLFW_KEY_A && action == GLFW_PRESS || keysUnreleased->at(GLFW_KEY_A) == true)
			{
				pos[0] += 0.1f;
				keysUnreleased->at(GLFW_KEY_A) = true;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS || keysUnreleased->at(GLFW_KEY_D) == true)
			{
				pos[0] -= 0.1f;
				keysUnreleased->at(GLFW_KEY_D) = true;
			}

			if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			{
				keysUnreleased->at(GLFW_KEY_W) = false;
			}
			if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			{
				keysUnreleased->at(GLFW_KEY_S) = false;
			}
			if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			{
				keysUnreleased->at(GLFW_KEY_A) = false;
			}
			if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			{
				keysUnreleased->at(GLFW_KEY_D) = false;
			}
	
		}

		void UpdateProjection(VkExtent2D WidthHeight)
		{
			auto aspect = (float)WidthHeight.width / (float)WidthHeight.height;
			proj = glm::perspective(glm::radians(70.f), aspect, 0.1f, 200.f);
			proj[1][1] *= -1;
		}

		glm::mat4 UpdateModelView(glm::mat4 ObjectTransform)
		{
			view = glm::translate(glm::mat4(1.f), pos);
			model = ObjectTransform;
			return proj * view * model;
		}
		
		glm::vec3 pos{ 0.f, -6.f, -10.f };
		glm::mat4 model;
		glm::mat4 view = glm::translate(glm::mat4(1.f), pos);
		glm::mat4 proj = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 200.f);

		std::vector<bool> keysUnreleased[349]{};
	};
}
