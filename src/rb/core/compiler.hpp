#pragma once

#if defined(_MSC_VER)
    // https://learn.microsoft.com/en-us/cpp/overview/compiler-versions
	#define RB_COMPILER_MSVC
	#ifdef __clang__
		#define RB_COMPILER_CLANG
	#endif
	#define RB_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define RB_COMPILER_VERSION_MINOR (_MSC_VER % 100)
	#ifdef _MSC_FULL_VER
		#if _MSC_FULL_VER / 10000 == _MSC_VER
			#define RB_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
		#elif _MSC_FULL_VER / 100000 == _MSC_VER
			#define RB_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
		#else
			#define RB_COMPILER_VERSION_PATCH 0
		#endif
	#else
		#define RB_COMPILER_VERSION_PATCH 0
	#endif
#elif defined(__GNUC__)
	#define RB_COMPILER_GCC_LIKE
	#ifdef __MINGW32__
		#define RB_COMPILER_MINGW
	#elif defined(__clang__)
		#define RB_COMPILER_CLANG
	#else
		#define RB_COMPILER_GCC
	#endif
	#ifdef RB_COMPILER_CLANG
		#define RB_COMPILER_VERSION_MAJOR __clang_major__
		#define RB_COMPILER_VERSION_MINOR __clang_minor__
		#define RB_COMPILER_VERSION_PATCH __clang_patchlevel__
		#define RB_COMPILER_VERSION_STRING __clang_version__
	#else
		#define RB_COMPILER_VERSION_MAJOR __GNUC__
		#define RB_COMPILER_VERSION_MINOR __GNUC_MINOR__
		#ifdef __GNUC_PATCHLEVEL__
			#define RB_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
		#else
			#define RB_COMPILER_VERSION_PATCH 0
		#endif
		#define RB_COMPILER_VERSION_STRING __VERSION__
	#endif
#else
	#error Unsupported compiler
#endif
