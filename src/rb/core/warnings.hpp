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
	#define RB_WARNING_STRING_LITERAL_OPERATOR_TEMPLATE \
		_Pragma(RB_STRINGIFY(clang diagnostic ignored "-Wgnu-string-literal-operator-template"))
	#define RB_WARNING_POSSIBLE_NULL_ARGUMENT
	#define RB_WARNING_POSSIBLE_NULL_DEREFERENCE
	#define RB_WARNING_POTENTIALLY_THROWING_FUNCTION
	#define RB_WARNING_ENUM_SWITCH
	#define RB_WARNING_PADDING \
		_Pragma(RB_STRINGIFY(clang diagnostic ignored "-Wpadded"))
#elif defined(RB_COMPILER_GCC_LIKE)
	#define RB_WARNING_DEPRECATED \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wdeprecated-declarations"))
	#define RB_WARNING_STRING_LITERAL_OPERATOR_TEMPLATE \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wpedantic"))
	#define RB_WARNING_POSSIBLE_NULL_ARGUMENT \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wanalyzer-possible-null-argument"))
	#define RB_WARNING_POSSIBLE_NULL_DEREFERENCE \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wanalyzer-possible-null-dereference"))
	#define RB_WARNING_POTENTIALLY_THROWING_FUNCTION
	#define RB_WARNING_ENUM_SWITCH
	#define RB_WARNING_PADDING \
		_Pragma(RB_STRINGIFY(GCC diagnostic ignored "-Wpadded"))
#elif defined(RB_COMPILER_MSVC)
	#define RB_WARNING_DEPRECATED \
		__pragma(warning(disable : 5039))
	#define RB_WARNING_STRING_LITERAL_OPERATOR_TEMPLATE
	#define RB_WARNING_POSSIBLE_NULL_ARGUMENT
	#define RB_WARNING_POSSIBLE_NULL_DEREFERENCE \
		__pragma(warning(disable : 6011))
	#define RB_WARNING_POTENTIALLY_THROWING_FUNCTION \
		__pragma(warning(disable : 5039))
	#define RB_WARNING_ENUM_SWITCH \
		__pragma(warning(disable : 4061))
	#define RB_WARNING_PADDING \
		__pragma(warning(disable : 4820))
#endif
