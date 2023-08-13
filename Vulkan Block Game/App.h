#pragma once
#include "Core/Vulkan/Core.h"
#include "Core/Game Components/Player.h"

extern Player* playerInstance;
extern std::map<int, bool> KeyPressed;

class App
{
public:
	App();
	~App() {};

	void Start()
	{
		MainLoop();
	}
	void Start(uint32_t WIDTH, uint32_t HEIGHT)
	{
		WINWIDTH = WIDTH;
		WINHEIGHT = HEIGHT;
		Start();
	}

private:
	void MainLoop();

public:
	bool shouldWindowClose = false;

	CORE core{ "VULKAN_TEMPLATE", frameinfo };
	Player player{ core.GetWindow(), frameinfo };

	GameObject::Map gameObjects{};
	std::vector<std::unique_ptr<Texture>> textures;
	FrameInfo frameinfo{ gameObjects, textures };

private:	
	
};

