#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/Decay.hpp>
#include <rb/core/traits/IsConvertible.hpp>

namespace rb::core {

/// Returns `RB_FWD(value)` (implicitly converted to the decayed type), a decayed prvalue copy of @p value.
/// @see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0849r8.html
template <class T,
    RB_REQUIRES(isConvertible<T, Decay<T>>)>
constexpr Decay<T> decayCopy(T&& value) noexcept(isNothrowConvertible<T, Decay<T>>) {
	return RB_FWD(value);
}

} // namespace rb::core
