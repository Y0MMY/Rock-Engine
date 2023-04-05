#include "pch.h"
#include "Core.h"

#define BUILD_ID "v0.1a"

namespace RockEngine
{
	void InitializeCore()
	{
		RockEngine::Log::Init();

		RE_CORE_TRACE("BUILD VERSION: {}", BUILD_ID);
	}
}