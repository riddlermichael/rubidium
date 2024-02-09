#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/requires.hpp>
#include <rb/sync/annotations.hpp>
#include <rb/sync/traits.hpp>

namespace rb::sync {

template <class Lockable,
    RB_REQUIRES(isBasicLockable<Lockable>)>
class RB_SCOPED_CAPABILITY LockGuard final {
public:
	explicit LockGuard(Lockable& lockable) RB_ACQUIRE_CAPABILITY(lockable, lockable_)
	    : lockable_(lockable) {
		lockable_.lock();
	}

	~LockGuard() RB_RELEASE_CAPABILITY() {
		lockable_.unlock();
	}

	RB_DISABLE_COPY(LockGuard);

	// Must be public for using with ConditionVariable
	Lockable const& lockable() const noexcept RB_RETURN_CAPABILITY(lockable_) {
		return lockable_;
	}

	Lockable& lockable() noexcept RB_RETURN_CAPABILITY(lockable_) {
		return lockable_;
	}

private:
	Lockable& lockable_;
};

} // namespace rb::sync
