#pragma once

#include <rb/core/compiler.hpp>
#include <rb/core/helpers.hpp>

#if defined(RB_COMPILER_CLANG)
	#define RB_WARNING_PUSH _Pragma("clang diagnostic push")
	#define RB_WARNING_POP _Pragma("clang diagnostic pop")
#elif defined(RB_COMPILER_GCC_LIKE)
	#define RB_WARNING_PUSH _Pragma("GCC diagnostic push")
	#define RB_WARNING_POP _Pragma("GCC diagnostic pop")
#elif defined(RB_COMPILER_MSVC)
	#define RB_WARNING_PUSH __pragma(warning(push))
	#define RB_WARNING_POP __pragma(warning(pop))
#endif

#if defined(RB_COMPILER_CLANG)
	#define RB_WARNING_DEPRECATED \
		_Pragma(RB_STRINGIFY(clang diagnostic ignored "-Wdeprecated-declarations"))
	#define RB_STRING_LITERAL_OPERATOR_TEMPLATE \
		_Pragma(RB_STRINGIFY(clang diagnostic ignored "-Wgnu-string-literal-operator-template"))
	#define RB_WARNING_SWITCH_ENUM \
		_Pragma(RB_STRINGIFY(clang diagnostic ignored "-Wswitch-enum"))
#elif defined(RB_COMPILER_GCC_LIKE)
	#define RB_WARNING_DEPRECATED \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wdeprecated-declarations"))
	#define RB_STRING_LITERAL_OPERATOR_TEMPLATE \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wpedantic"))
	#define RB_WARNING_SWITCH_ENUM \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wswitch-enum"))
#elif defined(RB_COMPILER_MSVC)
	#define RB_WARNING_DEPRECATED
	#define RB_STRING_LITERAL_OPERATOR_TEMPLATE
	#define RB_WARNING_SWITCH_ENUM
#endif
