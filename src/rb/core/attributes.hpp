#pragma once

#include <rb/core/compiler.hpp>

#ifdef RB_COMPILER_MSVC
	#include <sal.h>
#endif

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

#if defined(RB_COMPILER_GCC_LIKE)
// Clang also allows the nonnull attribute to be placed directly on a function parameter;
// use `RB_ARG_NONNULL` in a such case
	#define RB_NONNULL __attribute__((nonnull))
	#define RB_NONNULL_ARGS(...) __attribute__((nonnull(__VA_ARGS__)))
	#define RB_RETURNS_NONNULL __attribute__((returns_nonnull))
	#define RB_ALLOCATOR __attribute__((malloc))
// `alloc_size(N)` implies that argument number `N` equals the number of available bytes at the returned pointer
// (used for `malloc`-like functions).
// `alloc_size(N, M)` implies that the product of argument number `N` and argument number `M`
// equals the number of available bytes at the returned pointer (used for `calloc`-like functions).
// Argument numbers are 1-based.
	#define RB_ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
#elif defined(RB_COMPILER_MSVC)
	#define RB_NONNULL
	#define RB_NONNULL_ARGS(...)
	#define RB_RETURNS_NONNULL _Ret_notnull_
	#define RB_ALLOCATOR __declspec(allocator)
	#define RB_ALLOC_SIZE(...)
#else
	#define RB_NONNULL
	#define RB_NONNULL_ARGS(...)
	#define RB_RETURNS_NONNULL
	#define RB_ALLOCATOR
	#define RB_ALLOC_SIZE(...)
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

#if defined(RB_COMPILER_GCC_LIKE)
	#define RB_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(RB_COMPILER_MSVC)
	#define RB_ALWAYS_INLINE __forceinline
#else
	#define RB_ALWAYS_INLINE inline
#endif
