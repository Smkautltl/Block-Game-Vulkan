#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vulkan
{
	class camera
	{
	public:
		camera()
		{
			VK_CORE_INFO("Camera Created!")
			for (auto i = 0; i < 350; i++)
			{
				keysUnreleased.push_back(false);
			}
		}
		
		void UpdateCameraPos(int key, int scancode, int action, int mods)
		{
			up = rot * glm::vec4{ 0.f, 1.f, 0.f, 0.f };
			
			glm::vec3 forward_rot = rot * glm::vec4{ 0.f, 0.f, -1.f, 0.f };

			glm::vec3 right_rot = rot * glm::vec4{ 1.f, 0.f, 0.f, 0.f };

			if (key == GLFW_KEY_W && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_W] == true)
			{
				pos += forward_rot;
				keysUnreleased.at(GLFW_KEY_W) = true;
			}
			if (key == GLFW_KEY_S && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_S] == true)
			{
				pos -= forward_rot;
				keysUnreleased.at(GLFW_KEY_S) = true;
			}
			if (key == GLFW_KEY_A && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_A] == true)
			{
				pos -= right_rot;
				keysUnreleased.at(GLFW_KEY_A) = true;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_D] == true)
			{
				pos += right_rot;
				keysUnreleased.at(GLFW_KEY_D) = true;
			}

			if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			{
				keysUnreleased.at(GLFW_KEY_W) = false;
			}
			if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			{
				keysUnreleased.at(GLFW_KEY_S) = false;
			}
			if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			{
				keysUnreleased.at(GLFW_KEY_A) = false;
			}
			if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			{
				keysUnreleased.at(GLFW_KEY_D) = false;
			}
	
		}

		void UpdateCameraRot(float xpos, float ypos)
		{
			float SPEED_X = 0.00001f;
			float SPEED_Y = 0.00001f;
			
			float pos_x_delta = oldXpos - xpos;
			float pos_y_delta = oldYpos - ypos;

			if(pos_x_delta != 0.f)
			{
				y_Rot += glm::radians(pos_x_delta * SPEED_X);
				rot = glm::rotate(rot, y_Rot, glm::vec3(0.f, 1.f, 0.f));
			}
			if (pos_y_delta != 0.f)
			{
				x_Rot += glm::radians(pos_y_delta * SPEED_Y);
				rot = glm::rotate(rot, x_Rot, glm::vec3(1.f, 0.f, 0.f));
			}

			oldXpos = xpos;
			oldYpos = ypos;
		}

		void UpdateProjection(VkExtent2D WidthHeight)
		{
			auto aspect = (float)WidthHeight.width / (float)WidthHeight.height;
			proj = glm::perspective(glm::radians(70.f), aspect, 0.1f, 1000.f);
			proj[1][1] *= -1;
		}

		glm::mat4 UpdateModelView(glm::mat4 ObjectTransform)
		{
			glm::vec4 forward_rotated = rot * glm::vec4{ 0.f, 0.f, -1.f, 0.f};
			glm::vec3 forward_rot_3{forward_rotated.x, forward_rotated.y, forward_rotated.z};
			target = pos += glm::normalize(forward_rot_3);

			view = glm::lookAt(pos, target, up);
			
			//view = glm::translate(glm::mat4(1.f), pos);
			model = ObjectTransform;
			return proj * view * model;
		}
		
		glm::vec3 pos{ 0.f, 80.f, -5.f};
		glm::mat4 rot = glm::mat4(1.f);
		glm::vec3 up{ 0.f, 1.f, 0.f };
		glm::vec3 target = pos + glm::vec3{ 0.f, 0.f, -1.f};

		float oldXpos, oldYpos;
		float y_Rot, x_Rot;
		
		glm::mat4 model;
		glm::mat4 view = glm::lookAt(pos, target, up);
		glm::mat4 proj = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 1000.f);

		std::vector<bool> keysUnreleased;
	};
}
