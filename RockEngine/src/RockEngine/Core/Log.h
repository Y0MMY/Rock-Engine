#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace RockEngine
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
}

// Core message define
#define RE_CORE_TRACE(...)					RockEngine::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define RE_CORE_INFO(...)					RockEngine::Log::GetCoreLogger()->info(__VA_ARGS__);
#define RE_CORE_ERROR(...)					RockEngine::Log::GetCoreLogger()->error(__VA_ARGS__);