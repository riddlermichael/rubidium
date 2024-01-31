#pragma once

#ifdef __has_attribute
	#define RB_HAS_ATTRIBUTE(...) __has_attribute(__VA_ARGS__)
#else
	#define RB_HAS_ATTRIBUTE(...) 0
#endif

#ifdef __has_c_attribute
	#define RB_HAS_C_ATTRIBUTE(...) __has_c_attribute(__VA_ARGS__)
#else
	#define RB_HAS_C_ATTRIBUTE(...) 0
#endif

#ifdef __has_cpp_attribute
	#define RB_HAS_CPP_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
#else
	#define RB_HAS_CPP_ATTRIBUTE(...) 0
#endif

#ifdef __has_declspec_attribute
	#define RB_HAS_DECLSPEC_ATTRIBUTE(...) __has_declspec_attribute(__VA_ARGS__)
#else
	#define RB_HAS_DECLSPEC_ATTRIBUTE(...) 0
#endif

/*
 * The `lifetimebound` attribute on a function parameter or implicit object parameter
 * indicates that objects that are referred to by that parameter may also be referred to
 * by the return value of the annotated function
 * (or, for a parameter of a constructor, by the value of the constructed object).
 */
#if RB_HAS_CPP_ATTRIBUTE(clang::lifetimebound)
	#define RB_ATTR_LIFETIME_BOUND [[clang::lifetimebound]]
#elif RB_HAS_ATTRIBUTE(lifetimebound)
	#define RB_ATTR_LIFETIME_BOUND __attribute__((lifetimebound))
#else
	#define RB_ATTR_LIFETIME_BOUND
#endif
