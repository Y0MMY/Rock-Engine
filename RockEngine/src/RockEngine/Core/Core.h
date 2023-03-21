#pragma once

#include <memory>
#include "Ref.h"

namespace RockEngine
{
	void InitializeCore();
}


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