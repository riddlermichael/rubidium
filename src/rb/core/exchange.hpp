#pragma once

#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/constructible.hpp>

namespace rb::core {

template <class T, class U = T,
    RB_REQUIRES(isMoveConstructible<T>&& isAssignable<T&, U>)>
constexpr T exchange(T& obj, U&& value) noexcept(isNothrowMoveConstructible<T> && isNothrowAssignable<T&, U>) {
	T old = RB_MOVE(obj);
	obj = RB_FWD(value);
	return old;
}

} // namespace rb::core
