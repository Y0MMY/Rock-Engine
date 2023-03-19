#pragma once

#include <memory>

namespace RockEngine
{
	void InitializeCore();
}


#define BIT(x) (1u << x)

#include "REAssert.h"

namespace RockEngine
{
	using byte = unsigned char;
	using u32 = unsigned int;
}