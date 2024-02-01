#pragma once

#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/ops.hpp>

namespace rb::sync {

namespace impl {
	template <class T>
	using LockDetector = decltype(RB_DECLVAL(T).lock());

	template <class T>
	using UnlockDetector = decltype(RB_DECLVAL(T).unlock());

	template <class T>
	using TryLockDetector = decltype(RB_DECLVAL(T).tryLock());
} // namespace impl

template <class T>
using IsBasicLockable = core::And<
    core::IsDetected<impl::LockDetector, T>,
    core::IsDetected<impl::UnlockDetector, T>>;

template <class T>
inline constexpr bool isBasicLockable = IsBasicLockable<T>::value;

template <class T>
using IsLockable = core::And<
    IsBasicLockable<T>,
    core::IsDetected<impl::TryLockDetector, T>>;

template <class T>
inline constexpr bool isLockable = IsLockable<T>::value;

} // namespace rb::sync
