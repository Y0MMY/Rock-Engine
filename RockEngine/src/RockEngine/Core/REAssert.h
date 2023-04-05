#pragma once

#ifdef RE_DEBUG
#define RE_ENABLE_ASSERTS
#endif

#ifdef RE_ENABLE_ASSERTS
#define RE_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { RE_ERROR("Assertion Failed"); __debugbreak(); } }
#define RE_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { RE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define RE_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define RE_GET_ASSERT_MACRO(...) RE_EXPAND_VARGS(RE_ASSERT_RESOLVE(__VA_ARGS__, RE_ASSERT_MESSAGE, RE_ASSERT_NO_MESSAGE))

#define RE_ASSERT(...) RE_EXPAND_VARGS( RE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define RE_CORE_ASSERT(...) RE_EXPAND_VARGS( RE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define RE_ASSERT(...)
#define RE_CORE_ASSERT(...)
#endif