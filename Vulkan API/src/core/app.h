#pragma once
//-=-=-=-=- CORE -=-=-=-=-
#include "window.h"
#include "device.h"
#include "Renderer.h"
#include "game_object.h"
#include "camera.h"

//-=-=-=-=- STD -=-=-=-=-
#include <vector>

#include "../game/chunk.hpp"

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
		camera cam{};
		std::vector<Chunk> chunks;
	};
}
