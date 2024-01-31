#pragma once

#include <rb/core/compiler.hpp>

// Generic helper definitions for shared library support
#if defined(RB_COMPILER_MSVC)
	#define RB_DECL_IMPORT __declspec(dllimport)
	#define RB_DECL_EXPORT __declspec(dllexport)
	#define RB_DECL_HIDDEN
#elif defined(RB_COMPILER_GCC_LIKE)
	#define RB_DECL_IMPORT __attribute__((visibility("default")))
	#define RB_DECL_EXPORT __attribute__((visibility("default")))
	#define RB_DECL_HIDDEN __attribute__((visibility("hidden")))
#else
	#define RB_DECL_IMPORT
	#define RB_DECL_EXPORT
	#define RB_DECL_HIDDEN
#endif

// Now we use the generic helper definitions above to define RB_EXPORT and RB_HIDDEN.
// RB_EXPORT is used for the public API symbols;
// it either DLL imports or DLL exports (or does nothing for static build).
// RB_HIDDEN is used for non-api symbols.
#ifdef RB_STATIC
	#define RB_EXPORT
	#define RB_HIDDEN
#else
	#ifndef RB_EXPORT
		#ifdef RB_EXPORTS
			#define RB_EXPORT RB_DECL_EXPORT
		#else
			#define RB_EXPORT RB_DECL_IMPORT
		#endif
	#endif
	#define RB_HIDDEN RB_DECL_HIDDEN
#endif
