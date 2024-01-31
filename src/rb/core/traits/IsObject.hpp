#pragma once

#include <rb/core/traits/IsFunction.hpp>
#include <rb/core/traits/IsRef.hpp>
#include <rb/core/traits/IsVoid.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	inline constexpr bool isObject = !(isFunction<T> || isVoid<T> || isRef<T>);

	template <class T>
	using IsObject = Bool<isObject<T>>;

} // namespace traits
} // namespace rb::core
