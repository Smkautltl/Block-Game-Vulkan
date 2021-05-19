#pragma once
#include "spdlog/spdlog.h"

#define VK_ASSERT(x, ...)
#define VK_CORE_ASSERT(x, ...)

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

//Core log macros
#define VK_CORE_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define VK_CORE_INFO(...)  ::Log::GetCoreLogger()->info(__VA_ARGS__);
#define VK_CORE_WARN(...)  ::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VK_CORE_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__);

//Client log macros
#define VK_TRACE(...) ::Log::GetClientLogger()->trace(__VA_ARGS__);
#define VK_INFO(...)  ::Log::GetClientLogger()->info(__VA_ARGS__);
#define VK_WARN(...)  ::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VK_ERROR(...) ::Log::GetClientLogger()->error(__VA_ARGS__);