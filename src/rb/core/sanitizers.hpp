#pragma once

#include <rb/core/compiler.hpp>
#include <rb/core/features.hpp>

#if defined(__has_feature) // clang
	#if __has_feature(thread_sanitizer)
		#define RB_FEATURE_THREAD_SANITIZER 1
	#else
		#define RB_FEATURE_THREAD_SANITIZER (-1)
	#endif
#elif defined(__SANITIZE_THREAD__) // gcc
	#define RB_FEATURE_THREAD_SANITIZER 1
#else
	#define RB_FEATURE_THREAD_SANITIZER (-1)
#endif

#if defined(__has_feature) // clang
	#if __has_feature(address_sanitizer)
		#define RB_FEATURE_ADDRESS_SANITIZER 1
	#else
		#define RB_FEATURE_ADDRESS_SANITIZER (-1)
	#endif
#elif defined(__SANITIZE_ADDRESS__) // gcc
	#define RB_FEATURE_ADDRESS_SANITIZER 1
#else
	#define RB_FEATURE_ADDRESS_SANITIZER (-1)
#endif

#if defined(__has_feature) // clang
	#if __has_feature(memory_sanitizer)
		#define RB_FEATURE_MEMORY_SANITIZER 1
	#else
		#define RB_FEATURE_MEMORY_SANITIZER (-1)
	#endif
#else // MSAN unsupported by GCC
	#define RB_FEATURE_MEMORY_SANITIZER (-1)
#endif

#if defined(__has_feature) // clang
	#if __has_feature(undefined_behavior_sanitizer)
		#define RB_FEATURE_UB_SANITIZER 1
	#else
		#define RB_FEATURE_UB_SANITIZER (-1)
	#endif
#elif defined(__SANITIZE_UNDEFINED__) // pick up from CMake
	#define RB_FEATURE_UB_SANITIZER 1
#else
	#define RB_FEATURE_UB_SANITIZER (-1)
#endif

#ifdef RB_COMPILER_GCC_LIKE
	#define RB_NO_SANITIZE(check) __attribute__((no_sanitize(check)))
#else
	#define RB_NO_SANITIZE(check)
#endif

#ifdef RB_COMPILER_CLANG
	#define RB_NO_SANITIZE_UNSIGNED_INTEGER_OVERFLOW __attribute__((no_sanitize("unsigned-integer-overflow")))
#else
	#define RB_NO_SANITIZE_UNSIGNED_INTEGER_OVERFLOW
#endif
