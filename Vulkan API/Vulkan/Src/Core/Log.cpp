#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

void Log::Init()
{
	//Sets the pattern to [Time] Name of logger: Message
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_CoreLogger = spdlog::stdout_color_mt("Vulkan API");
	s_CoreLogger->set_level(spdlog::level::info);

	s_ClientLogger = spdlog::stdout_color_mt("Minecraft Clone");
	s_ClientLogger->set_level(spdlog::level::trace);
}