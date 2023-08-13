#pragma once
#include "Core/Vulkan/Utils.h"
#include "GameObject.h"
#include "Camera.h"
#include "Input.h"

class Player
{
public:
    Player(Window& window, FrameInfo& Frameinfo);

    Camera& GetCamera() { return camera; }
    void Update();

private:
    static void MoveLeft();
    static void MoveRight();
    static void MoveForward();
    static void MoveBackward();
    static void MoveUp();
    static void MoveDown();

    static void Look(GLFWwindow* window, double xpos, double ypos);

public:
   
private:
    Window& window;
    Input input{};
	Camera camera{};
    GameObject obj = GameObject::CreateGameObject();

    FrameInfo& frameinfo;

    float moveSpeed{ 0.05f };
    float lookSpeed{ 1.f };

    float KeyCoolDown = 0;
    double curMouseX, curMouseY;
    double prevMouseX, prevMouseY;
};

extern Player* playerInstance;