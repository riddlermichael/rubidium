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
	#define RB_LIFETIME_BOUND [[clang::lifetimebound]]
#elif RB_HAS_ATTRIBUTE(lifetimebound)
	#define RB_LIFETIME_BOUND __attribute__((lifetimebound))
#else
	#define RB_LIFETIME_BOUND
#endif

#if defined(RB_COMPILER_GCC_LIKE)
	#define RB_ALWAYS_INLINE __attribute__((always_inline))
	#if RB_HAS_ATTRIBUTE(noipa)
		#define RB_NEVER_INLINE __attribute__((noinline, noipa))
	#else
		#define RB_NEVER_INLINE __attribute__((noinline))
	#endif
#elif defined(RB_COMPILER_MSVC)
	#define RB_ALWAYS_INLINE __forceinline
	#define RB_NEVER_INLINE __declspec(noinline)
#else
	#define RB_ALWAYS_INLINE inline
	#define RB_NEVER_INLINE
#endif

#if defined(RB_COMPILER_GCC_LIKE)
    /**
     * Calls to functions whose return value is not affected by changes to the observable state of the program
     * and that have no observable effects on such state other than to return a value may lend themselves to optimizations
     * such as common subexpression elimination.
     * Declaring such functions with the `const` attribute allows to avoid emitting some calls
     * in repeated invocations of the function with the same argument values.
     *
     * The `const` attribute prohibits a function from reading objects that affect its return value
     * between successive invocations.
     * However, functions declared with the attribute can safely read objects that do not change their return value,
     * such as non-volatile constants.
     *
     * The `const` attribute imposes greater restrictions on a function’s definition than the similar `pure` attribute.
     * Because a `const` function cannot have any observable side effects it does not make sense for it to return `void`.
     *
     * Note that a function that has pointer arguments and examines the data pointed to **must not be** declared `const`
     * if the pointed-to data might change between successive invocations of the function.
     * In general, since a function cannot distinguish data that might change from data that cannot,
     * `const` functions should never take pointer or, in C++, reference arguments.
     * Likewise, a function that calls a non-const function usually must not be `const` itself.
     */
	#define RB_CONST __attribute__((const))

    /**
     * Calls to functions that have no observable effects on the state of the program other than to return a value
     * may lend themselves to optimizations such as common subexpression elimination.
     * Declaring such functions with the `pure` attribute allows to avoid emitting some calls
     * in repeated invocations of the function with the same argument values.
     *
     * The `pure` attribute prohibits a function from modifying the state of the program that is observable
     * by means other than inspecting the function’s return value.
     * However, functions declared with the `pure` attribute can safely read any non-volatile objects,
     * and modify the value of objects in a way that does not affect their return value
     * or the observable state of the program.
     *
     * Some common examples of pure functions are `strlen` or `memcmp`.
     * Interesting non-`pure` functions are functions with infinite loops or those depending on volatile memory
     * or other system resource, that may change between consecutive calls
     * (such as the standard C `feof` function in a multithreading environment).
     *
     * The pure attribute imposes similar but looser restrictions on a function’s definition than the `const` attribute:
     * `pure` allows the function to read any non-volatile memory, even if it changes in between successive invocations
     * of the function.
     * Because a `pure` function cannot have any observable side effects it does not make sense for such a function
     * to return `void`.
     */
	#define RB_PURE __attribute__((pure))
#else
	#define RB_CONST
	#define RB_PURE
#endif
