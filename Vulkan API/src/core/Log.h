#pragma once
//-=-=-=-=- SPDLOG -=-=-=-=-
#include <spdlog/spdlog.h>

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	//static int StartTimePoint(std::string msg)
	//{
	//	std::chrono::time_point<std::chrono::system_clock> start;
	//	start = std::chrono::system_clock::now();
	//	time_points_.emplace_back(start, msg);
	//	return time_points_.size() - 1;
	//}
	//static void EndTimePoint(int id)
	//{
	//	auto& point = time_points_[id];
	//	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - point.first;
	//
	//	GetCoreLogger()->info(point.second + " completed in " + std::to_string(elapsed_seconds.count()) + "s");
	//}

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;

	//static std::vector<std::pair<std::chrono::time_point<std::chrono::system_clock>, std::string>> time_points_;
	
};

//Assert log macros
#ifdef NDEBUG
#define VK_ASSERT(check, msg, ...);
#define VK_CORE_ASSERT(check, msg, ...);
#else
#define VK_ASSERT(check, msg, ...) { if(!(check)){VK_CRITICAL(msg) __debugbreak();} };
#define VK_CORE_ASSERT(check, msg, ...) { if(!(check)){VK_CORE_CRITICAL(msg) __debugbreak();} };
#endif

//Core log macros
#ifdef NDEBUG
#define VK_CORE_TRACE(...);
#define VK_CORE_INFO(...);
#define VK_CORE_WARN(...);
#define VK_CORE_ERROR(...);
#define VK_CORE_CRITICAL(...);
#define VK_CORE_RUNTIME(...);
#else
#define VK_CORE_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define VK_CORE_INFO(...)  ::Log::GetCoreLogger()->info(__VA_ARGS__);
#define VK_CORE_WARN(...)  ::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VK_CORE_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__);
#define VK_CORE_CRITICAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__);
#define VK_CORE_RUNTIME(x) VK_CORE_CRITICAL(x) throw std::runtime_error("");

//#define VK_CORE_BENCH_START(msg) ::Log::StartTimePoint(msg);
//#define VK_CORE_BENCH_END(id) ::Log::EndTimePoint(id);
#endif

//Client log macros
#define VK_TRACE(...) ::Log::GetClientLogger()->trace(__VA_ARGS__);
#define VK_INFO(...)  ::Log::GetClientLogger()->info(__VA_ARGS__);
#define VK_WARN(...)  ::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VK_ERROR(...) ::Log::GetClientLogger()->error(__VA_ARGS__);
#define VK_CRITICAL(...) ::Log::GetClientLogger()->critical(__VA_ARGS__);