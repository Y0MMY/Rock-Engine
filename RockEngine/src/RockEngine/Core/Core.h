#pragma once

#include <memory>
#include "RockEngine/Memory/Ref.h"

#define BUILD_ID "v0.1a"

namespace RockEngine
{
	using namespace Memory;
	void InitializeCore();
}

// __VA_ARGS__ expansion to get past MSVC "bug"
#define RE_EXPAND_VARGS(x) x

#define BIT(x) (1u << x)
#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) \
{ \
	return this->fn(std::forward<decltype(args)>(args)...); \
}

#include "REAssert.h"

namespace RockEngine
{
	using byte = unsigned char;
	using u32 = unsigned int;
}