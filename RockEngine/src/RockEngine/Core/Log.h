#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace RockEngine
{
	enum class Type : uint8_t
	{
		Core = 0, Client = 1
	};

	enum class Level : uint8_t
	{
		Trace = 0, Info, Warn, Error, Fatal
	};

	struct TagDetails
	{
		bool Enabled = true;
		Level LevelFilter;
	};

	class Log
	{
	public:
		static void Init();
		static void Shutdown();
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
}

// Core Logging Macros
#define RE_CORE_TRACE(...)	RockEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RE_CORE_INFO(...)	RockEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RE_CORE_WARN(...)	RockEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RE_CORE_ERROR(...)	RockEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RE_CORE_FATAL(...)	RockEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// CLient Logging Macros
#define RE_TRACE(...)		RockEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RE_INFO(...)		RockEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RE_WARN(...)		RockEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RE_ERROR(...)		RockEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RE_FATAL(...)		RockEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

