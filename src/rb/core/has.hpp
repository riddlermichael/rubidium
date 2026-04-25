#pragma once

#ifndef RB_HAS_BUILTIN
	#ifdef __has_builtin
		#define RB_HAS_BUILTIN(x) __has_builtin(x)
	#else
		#define RB_HAS_BUILTIN(x) 0
	#endif
#endif

#ifndef RB_HAS_ATTRIBUTE
	#ifdef __has_attribute
		#define RB_HAS_ATTRIBUTE(...) __has_attribute(__VA_ARGS__)
	#else
		#define RB_HAS_ATTRIBUTE(...) 0
	#endif
#endif

#ifndef RB_HAS_CPP_ATTRIBUTE
	#ifdef __has_cpp_attribute
		#define RB_HAS_CPP_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
	#else
		#define RB_HAS_CPP_ATTRIBUTE(...) 0
	#endif
#endif

#ifndef RB_HAS_DECLSPEC_ATTRIBUTE
	#ifdef __has_declspec_attribute
		#define RB_HAS_DECLSPEC_ATTRIBUTE(...) __has_declspec_attribute(__VA_ARGS__)
	#else
		#define RB_HAS_DECLSPEC_ATTRIBUTE(...) 0
	#endif
#endif
