#include "Log.h"

//-=-=-=-=- SPDLOG -=-=-=-=-
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
	//Sets the pattern to [Time] Name of logger: Message
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_CoreLogger = spdlog::stdout_color_mt("Vulkan API");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stdout_color_mt("Block Game");
	s_ClientLogger->set_level(spdlog::level::trace);
}