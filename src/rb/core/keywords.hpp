#pragma once

#ifdef __cpp_consteval
	#define RB_CONSTEVAL consteval
#else
	#define RB_CONSTEVAL constexpr
#endif

#ifdef __cpp_constexpr_dynamic_alloc
	#define RB_CONSTEXPR20 constexpr
#else
	#define RB_CONSTEXPR20 inline
#endif
