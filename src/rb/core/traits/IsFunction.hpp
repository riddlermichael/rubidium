#pragma once

#include <rb/core/traits/IsConst.hpp>
#include <rb/core/traits/IsRef.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	inline constexpr bool isFunction =
#if defined(RB_COMPILER_CLANG)
	    __is_function(T);
#else
	    !isConst<T const> && !isRef<T>;
#endif

	template <class T>
	using IsFunction = Bool<isFunction<T>>;

} // namespace traits
} // namespace rb::core
