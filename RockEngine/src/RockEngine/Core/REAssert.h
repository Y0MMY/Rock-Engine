#pragma once

#ifdef RE_DEBUG
#define RE_ENABLE_ASSERTS
#endif

#ifdef RE_ENABLE_ASSERTS
#define RE_CORE_ASSERT(x, ...) { if(!(x)) { RE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define RE_CORE_ASSERT(x, ...)
#endif