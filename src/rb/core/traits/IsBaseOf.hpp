#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class Base, class Derived>
	using IsBaseOf = Bool<__is_base_of(Base, Derived)>;

	template <class Base, class Derived>
	inline constexpr bool isBaseOf = __is_base_of(Base, Derived);

} // namespace traits
} // namespace rb::core
