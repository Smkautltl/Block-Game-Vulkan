#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "device.h"
#include "window.h"
#include "renderer.h"
#include "game_object.h"

//Must be below device and window headers 
#include "camera.h"

//-=-=-=-=- STD -=-=-=-=-
#include <vector>

namespace Vulkan
{
	class App
	{
	public:
		App();
		~App();

		//Deleting copy constructors
		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();

	private:
		void load_game_objects();

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

	private:
		Window window_{ WIDTH, HEIGHT, "Block Game" };
		Device device_{ window_ };
		Renderer renderer_{ window_, device_ };
		std::vector<GameObject> game_objects_;
		Camera cam{70.f, WIDTH, HEIGHT, 0.1f, 1000.f};
	};
}
