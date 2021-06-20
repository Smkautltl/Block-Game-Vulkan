#include "camera.h"

#include <GLFW/glfw3.h>

namespace Vulkan
{
	Camera::Camera(float FOV, float width, float height, float nearValue, float farValue)
	{
		{
			//VK_CORE_TRACE("Camera Created!")
			proj_matrix_ = glm::perspective(glm::radians(FOV), width / height, nearValue, farValue) * glm::mat4(-1.0f);
			view_matrix_ = glm::mat4(1.0f);

			cam_pos_ = glm::vec3(0.0f, 0.0f, 3.0f);
			cam_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
			cam_front_ = glm::vec3(0.0f, 0.0f, -1.0f);

			for (auto i = 0; i < 350; i++)
			{
				keysUnreleased.push_back(false);
			}
		}
	}

	void Camera::update_camera_pos(int key, int scancode, int action, int mods)
	{
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_W] == true)
			{
				cam_pos_ += cam_speed_ * cam_front_;
				keysUnreleased.at(GLFW_KEY_W) = true;
			}
		if (key == GLFW_KEY_S && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_S] == true)
			{
				cam_pos_ -= cam_speed_ * cam_front_;
				keysUnreleased.at(GLFW_KEY_S) = true;
			}
		if (key == GLFW_KEY_A && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_A] == true)
			{
				cam_pos_ -= glm::normalize(glm::cross(cam_front_, cam_up_)) * cam_speed_;
				keysUnreleased.at(GLFW_KEY_A) = true;
			}
		if (key == GLFW_KEY_D && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_D] == true)
			{
				cam_pos_ += glm::normalize(glm::cross(cam_front_, cam_up_)) * cam_speed_;
				keysUnreleased.at(GLFW_KEY_D) = true;
			}
		if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS || keysUnreleased[GLFW_KEY_LEFT_ALT] == true)
			keysUnreleased.at(GLFW_KEY_LEFT_ALT) = true;
		
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
		if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE)
			keysUnreleased.at(GLFW_KEY_LEFT_ALT) = false;
		recalculate_view_matrix();
	
	}

	void Camera::update_camera_rot(float xpos, float ypos)
	{
		if (keysUnreleased[GLFW_KEY_LEFT_ALT] == true)
		{
			set_yaw(xpos - old_x_pos_);
			set_pitch(old_y_pos_ - ypos);
			recalculate_camera_rotation();

		}

		old_x_pos_ = xpos;
		old_y_pos_ = ypos;
		
	}

	void Camera::recalculate_proj_matrix(float FOV, float width, float height, float nearValue, float farValue)
	{
		proj_matrix_ = glm::perspective(glm::radians(FOV), width / height, nearValue, farValue);
	}

	void Camera::recalculate_camera_rotation()
	{
		if (pitch_ > 89.0f)
			pitch_ = 89.0f;
		if (pitch_ < -89.0f)
			pitch_ = -89.0f;

		cam_rotation_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		cam_rotation_.y = sin(glm::radians(pitch_));
		cam_rotation_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

		cam_front_ = glm::normalize(cam_rotation_);
	}
	void Camera::recalculate_view_matrix()
	{
		view_matrix_ = glm::lookAt(cam_pos_, cam_pos_ + cam_front_, cam_up_);
	}
}
