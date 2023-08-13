#include "Player.h"

Player::Player(Window& window_, FrameInfo& Frameinfo) : window(window_), frameinfo(Frameinfo)
{
	input.SetKeyCallback(window);
	glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window.GetWindow(), Look);
	glfwGetCursorPos(window.GetWindow(), &prevMouseX, &prevMouseY);
	obj.transform.translation.z = -2;
}

void Player::Update()
{
	if (KeyPressed[GLFW_KEY_ESCAPE]) glfwSetWindowShouldClose(window.GetWindow(), true);

	if (KeyPressed[GLFW_KEY_A]) MoveLeft();
	if (KeyPressed[GLFW_KEY_D]) MoveRight();
	if (KeyPressed[GLFW_KEY_W]) MoveForward();
	if (KeyPressed[GLFW_KEY_S]) MoveBackward();

	if (KeyPressed[GLFW_KEY_SPACE]) MoveUp();
	if (KeyPressed[GLFW_KEY_LEFT_SHIFT]) MoveDown();

	if (KeyPressed[GLFW_KEY_F11] && KeyCoolDown >= 1.0f)
	{
		if (frameinfo.MakeFullscreen || frameinfo.MakeWindowed) return;

		if (frameinfo.isinWindowed)
			frameinfo.MakeFullscreen = true;
		else
			frameinfo.MakeWindowed = true;

		KeyPressed[GLFW_KEY_F11] = false;
		KeyCoolDown = 0;
	}


	//TODO: FIX UP STICKY MOVEMENT IN FULLSCREEN (POSSIBLY GIMBLE LOCKING)
	obj.transform.rotation += frameinfo.FrameTime * glm::vec3(
																-(curMouseY - prevMouseY) * (lookSpeed * frameinfo.aspect),
																 (curMouseX - prevMouseX) * (lookSpeed), 
																 0
															 );
	prevMouseX = curMouseX;
	prevMouseY = curMouseY;

	obj.transform.rotation.x = glm::clamp(obj.transform.rotation.x, -1.5f, 1.5f);
	obj.transform.rotation.y = glm::mod(obj.transform.rotation.y, glm::two_pi<float>());
	camera.SetViewYXZ(obj.transform.translation, obj.transform.rotation);

	frameinfo.Projection = camera.GetProjection();
	frameinfo.View = camera.GetView();
	frameinfo.InverseView = camera.GetInverseView();
	frameinfo.CameraPosition = camera.GetInverseView()[3];

	KeyCoolDown = KeyCoolDown > 1.0f ? 1.0f : KeyCoolDown + frameinfo.FrameTime;
}

void Player::MoveLeft()
{
	float yaw = playerInstance->obj.transform.rotation.y;
	glm::vec3 forwardDir(sin(yaw), 0.f, cos(yaw));
	glm::vec3 rightDir(forwardDir.z, 0.f, -forwardDir.x);
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(-rightDir);
}
void Player::MoveRight()
{
	float yaw = playerInstance->obj.transform.rotation.y;
	glm::vec3 forwardDir(sin(yaw), 0.f, cos(yaw));
	glm::vec3 rightDir(forwardDir.z, 0.f, -forwardDir.x);
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(rightDir);
}
void Player::MoveForward()
{
	float yaw = playerInstance->obj.transform.rotation.y;
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(glm::vec3(sin(yaw), 0.f, cos(yaw)));
}
void Player::MoveBackward()
{
	float yaw = playerInstance->obj.transform.rotation.y;
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(-(glm::vec3(sin(yaw), 0.f, cos(yaw))));
}
void Player::MoveUp()
{
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(glm::vec3(-0.f, -1.f, 0.f));
}
void Player::MoveDown()
{
	playerInstance->obj.transform.translation += playerInstance->moveSpeed * playerInstance->frameinfo.FrameTime * glm::normalize(-(glm::vec3(-0.f, -1.f, 0.f)));
}

void Player::Look(GLFWwindow* window, double xpos, double ypos)
{
	playerInstance->curMouseX = xpos;
	playerInstance->curMouseY = ypos;
}