#pragma once

#include <rb/core/traits/Constant.hpp>

namespace rb::core {
inline namespace traits {

	template <bool value>
	using Bool = Constant<value>;

	using True = Bool<true>;
	using False = Bool<false>;

} // namespace traits
} // namespace rb::core
