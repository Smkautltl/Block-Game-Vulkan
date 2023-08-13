#pragma once
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"

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
#ifdef NDEBUG
	#define VK_CORE_TRACE(...);
	#define VK_CORE_INFO(...);
	#define VK_CORE_WARN(...);
	#define VK_CORE_ERROR(...);
	#define VK_CORE_CRITICAL(...);
	#define VK_CORE_RUNTIME(...);
	#define VK_CORE_ASSERT(check, msg, ...);
	#define VK_CORE_CHECK_RESULT(x) if((x) != VK_SUCCESS) VK_CORE_CRITICAL("")
	struct VK_CORE_BENCH_SCOPE
	{
		std::string name_;
		VK_CORE_BENCH_SCOPE(std::string name) : name_(name) {};
		~VK_CORE_BENCH_SCOPE();
	};

#else
	#define VK_CORE_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__);
	#define VK_CORE_INFO(...)  ::Log::GetCoreLogger()->info(__VA_ARGS__);
	#define VK_CORE_WARN(...)  ::Log::GetCoreLogger()->warn(__VA_ARGS__);
	#define VK_CORE_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__);
	#define VK_CORE_CRITICAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__);
	#define VK_CORE_ASSERT(check, msg, ...) { if(!(check)){VK_CORE_CRITICAL(msg) __debugbreak();} };
	#define VK_CORE_RUNTIME(x) VK_CORE_CRITICAL(x) throw std::runtime_error("");
	#define VK_CORE_CHECK_RESULT(x) if((x) != VK_SUCCESS) VK_CORE_CRITICAL("FATAL: VkResult FAILED in {0} at line {1}", __FILE__, __LINE__)

	struct VK_CORE_BENCH_SCOPE
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::string name_;
	VK_CORE_BENCH_SCOPE(std::string name) : name_(name)
	{
		start = std::chrono::high_resolution_clock::now();
	}
	~VK_CORE_BENCH_SCOPE()
	{
		std::chrono::duration<double> elapsed_seconds = std::chrono::high_resolution_clock::now() - start;
		auto count = elapsed_seconds.count() * 1000;
		if (count < 1.f)
		{
			VK_CORE_WARN("{0} took {1}ms ({2}us)", name_, std::to_string(count), std::to_string(count * 1000))
		}
		else
		{
			VK_CORE_WARN("{0} took {1}ms", name_, std::to_string(count))
		}

	}
};
	
#endif

//Client log macros
#define VK_TRACE(...) ::Log::GetClientLogger()->trace(__VA_ARGS__);
#define VK_INFO(...)  ::Log::GetClientLogger()->info(__VA_ARGS__);
#define VK_WARN(...)  ::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VK_ERROR(...) ::Log::GetClientLogger()->error(__VA_ARGS__);
#define VK_CRITICAL(...) ::Log::GetClientLogger()->critical(__VA_ARGS__);
#define VK_ASSERT(check, msg, ...) { if(!(check)){VK_CRITICAL(msg);} };

//-=-=-=- EXAMPLES -=-=-=-
//spdlog::info("Welcome to spdlog!");
//spdlog::error("Some error message with arg: {}", 1);
//
//spdlog::warn("Easy padding in numbers like {:08d}", 12);
//spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
//spdlog::info("Support for floats {:03.2f}", 1.23456);
//spdlog::info("Positional args are {1} {0}..", "too", "supported");
//spdlog::info("{:<30}", "left aligned");