#include "core/app.h"

//-=-=-=-=-=- STD -=-=-=-=-=-=-
#include <cstdlib>
#include <iostream>

int main()
{
	Vulkan::App app{};
	
	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}