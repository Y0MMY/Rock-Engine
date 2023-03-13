#include "pch.h"
#include "Log.h"

namespace RockEngine
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Init()
	{
		// change log pattern
		spdlog::set_pattern("%^[%T] %n: %v%$");

		// Core
		s_CoreLogger = spdlog::stdout_color_mt("RockEngine");
		s_CoreLogger->set_level(spdlog::level::trace);
	}
}
