#pragma once

#include <rb/core/requires.hpp>
#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsIntegral.hpp>

namespace rb::core {

/// Converts an enumeration to its underlying type.
template <class E,
    RB_REQUIRES(isEnum<E>)>
constexpr auto toUnderlying(E value) noexcept {
	return static_cast<UnderlyingType<E>>(value);
}

/// Converts an enumeration to integral type T.
template <class T, class E,
    RB_REQUIRES(isEnum<E>&& isIntegral<T>)>
constexpr auto toInt(E value) noexcept {
	return static_cast<T>(value);
}

/// Converts an enumeration to its underlying type.
template <class E,
    RB_REQUIRES(isEnum<E>)>
constexpr auto toInt(E value) noexcept {
	return static_cast<UnderlyingType<E>>(value);
}

} // namespace rb::core
