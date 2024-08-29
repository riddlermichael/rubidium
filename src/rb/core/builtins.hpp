#pragma once

#include <rb/core/compiler.hpp>

#ifdef __has_builtin
	#define RB_HAS_BUILTIN(x) __has_builtin(x)
#else
	#define RB_HAS_BUILTIN(x) 0
#endif

#if defined(RB_COMPILER_GCC_LIKE) || defined(RB_COMPILER_CLANG)
	#define RB_BUILTIN_FILE __builtin_FILE()
	#define RB_BUILTIN_LINE __builtin_LINE()
	#define RB_BUILTIN_FUNCTION __builtin_FUNCTION()
	#ifdef RB_COMPILER_CLANG
		#define RB_BUILTIN_COLUMN __builtin_COLUMN()
		#define RB_HAS_BUILTIN_COLUMN 1
	#else
		#define RB_BUILTIN_COLUMN 0
		#define RB_HAS_BUILTIN_COLUMN (-1)
	#endif
	#define RB_BUILTIN_FUNCSIG __builtin_FUNCTION()
	#define RB_HAS_BUILTIN_SOURCE_LOCATION 1
#elif defined(RB_COMPILER_MSVC)
	#if _MSC_VER >= 1926 && _MSVC_LANG > 201703L // works in C++20 mode only
		#define RB_BUILTIN_FILE __builtin_FILE()
		#define RB_BUILTIN_LINE __builtin_LINE()
		#define RB_BUILTIN_FUNCTION __builtin_FUNCTION()
		#define RB_BUILTIN_COLUMN __builtin_COLUMN()
		#define RB_BUILTIN_FUNCSIG __builtin_FUNCSIG()
		#define RB_HAS_BUILTIN_SOURCE_LOCATION 1
		#define RB_HAS_BUILTIN_COLUMN 1
	#else
		#define RB_BUILTIN_FILE __FILE__
		#define RB_BUILTIN_LINE __LINE__
		#define RB_BUILTIN_FUNCTION ""
		#define RB_BUILTIN_COLUMN 0
		#define RB_BUILTIN_FUNCSIG __FUNCSIG__
		#define RB_HAS_BUILTIN_SOURCE_LOCATION (-1)
		#define RB_HAS_BUILTIN_COLUMN (-1)
	#endif
#else
	#define RB_BUILTIN_FILE __FILE__
	#define RB_BUILTIN_LINE __LINE__
	#define RB_BUILTIN_FUNCTION __func__
	#define RB_BUILTIN_COLUMN 0
	#define RB_BUILTIN_FUNCSIG __func__
	#define RB_HAS_BUILTIN_SOURCE_LOCATION (-1)
	#define RB_HAS_BUILTIN_COLUMN (-1)
#endif

#if defined(RB_COMPILER_MSVC)
	#define RB_UNREACHABLE() __assume(0)
#elif defined(RB_COMPILER_GCC_LIKE)
	#define RB_UNREACHABLE() __builtin_unreachable()
#else

namespace rb::core {

[[noreturn]] inline void unreachableImpl() {
}

} // namespace rb::core

	#define RB_UNREACHABLE() rb::core::unreachableImpl()
#endif

#if defined(RB_COMPILER_GCC_LIKE)
	#define RB_UNLIKELY(x) __builtin_expect(!!(x), 0)
	#define RB_LIKELY(x) __builtin_expect(!!(x), 1)
#else
	#define RB_UNLIKELY(x) (x)
	#define RB_LIKELY(x) (x)
#endif

#if defined(RB_COMPILER_GCC_LIKE)
	#define RB_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(RB_COMPILER_MSVC)
	#define RB_CURRENT_FUNCTION __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define RB_CURRENT_FUNCTION __func__
#elif __cplusplus >= 201'103
	#define RB_CURRENT_FUNCTION __func__
#else
	#define RB_CURRENT_FUNCTION "(unknown)"
#endif
