#pragma once

//-=-=-=-=- STD -=-=-=-=-
#include <vector>

//-=-=-=-=- GLFW -=-=-=-=-
#define GLFORCE_RADIANS
#define GLFORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vulkan
{
	class Camera
	{
	public:
		Camera(float FOV, float width, float height, float nearValue, float farValue);
		
		void update_camera_pos(int key, int scancode, int action, int mods);
		
		void update_camera_rot(float xpos, float ypos);

		const glm::mat4& get_proj_matrix() const { return proj_matrix_; }
		const glm::mat4& get_view_matrix() const { return view_matrix_; }
		const glm::mat4& get_model_matrix() const { return model_matrix_; }

		const void set_proj_matrix(const glm::mat4& matrix) { proj_matrix_= matrix; }
		const void set_view_matrix(const glm::mat4& matrix) { view_matrix_= matrix; }
		const void set_model_matrix(const glm::mat4& matrix){ model_matrix_= matrix; }

		const glm::vec3& get_camera_front() const { return cam_front_; }
		const glm::vec3& get_camera_up() const { return cam_up_; }
		const glm::vec3& get_camera_pos() const { return cam_pos_; }
		

		void recalculate_view_matrix();
		void recalculate_proj_matrix(float FOV, float width, float height, float nearValue = 0.1f, float farValue = 1000.0f);
		void recalculate_camera_rotation();

		void set_position(const glm::vec3& cameraposition) {cam_pos_ = cameraposition; recalculate_view_matrix(); }
		void set_yaw(const float& yaw) {yaw_ += yaw * sensitvity_; recalculate_camera_rotation(); }
		void set_pitch(const float& pitch) {pitch_ += pitch * sensitvity_; recalculate_camera_rotation(); }
		
	private:
		glm::mat4 proj_matrix_{};
		glm::mat4 view_matrix_{};
		glm::mat4 model_matrix_{};
										    
		glm::vec3 cam_pos_{0.f, 100.f, 0.f};
		glm::vec3 cam_up_{};
		glm::vec3 cam_front_{};
		
		glm::vec3 cam_rotation_ =	{ 0.0f, 0.0f, 0.0f };
		glm::vec3 camera_target_ =	{ 0.0f, 0.0f, 0.0f };
		
		float yaw_ = 0.0f;
		float pitch_ = 0.0f;
		float sensitvity_ = 0.05f;
		float cam_speed_ = 0.1f;

		float old_x_pos_{}, old_y_pos_{};

		std::vector<bool> keysUnreleased{};
	};
}
