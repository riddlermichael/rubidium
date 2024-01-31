#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	struct IsConst : False {
	};

	template <class T>
	struct IsConst<T const> : True {
	};

	template <class T>
	inline constexpr bool isConst = IsConst<T>::value;

} // namespace traits
} // namespace rb::core
