#pragma once

#if defined(__clang__)
	#define RB_COMPILER_CLANG
	#define RB_COMPILER_VERSION_MAJOR __clang_major__
	#define RB_COMPILER_VERSION_MINOR __clang_minor__
	#define RB_COMPILER_VERSION_PATCH __clang_patchlevel__
	#define RB_COMPILER_VERSION_STRING __clang_version__
#elif defined(__GNUC__)
	#define RB_COMPILER_GCC
	#define RB_COMPILER_VERSION_MAJOR __GNUC__
	#define RB_COMPILER_VERSION_MINOR __GNUC_MINOR__
	#define RB_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
	#define RB_COMPILER_VERSION_STRING __VERSION__
	#ifdef __MINGW32__
		#define RB_COMPILER_MINGW
	#endif
#elif defined(_MSC_VER)
	#define RB_COMPILER_MSVC
	#define RB_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define RB_COMPILER_VERSION_MINOR (_MSC_VER % 100)
	#define RB_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#else
	#error Unsupported compiler
#endif

#ifdef __GNUC__
	#define RB_COMPILER_GCC_LIKE
#endif

#ifdef _MSC_VER
	#define RB_COMPILER_MSVC_LIKE
#endif
