#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/sync.hpp>
#include <rb/sync/LockGuard.hpp>

namespace rb::sync {

class RB_EXPORT RB_CAPABILITY("rb::sync::SharedMutex") SharedMutex {
public:
	enum Policy {
		kPreferReaders,
		kPreferWriters,
		kPreferWritersNonrecursive
	};

	/**
	 * Initializes a shared mutex object with the supplied @p policy.
	 * @param policy the policy used by this mutex; ignored on Windows
	 */
	explicit SharedMutex(Policy policy = kPreferReaders);
	~SharedMutex() noexcept(RB_USE(WIN32_THREADS));

	void lock() RB_ACQUIRE_CAPABILITY();
	void lockShared() RB_ACQUIRE_SHARED_CAPABILITY();
	bool tryLock() noexcept RB_TRY_ACQUIRE_CAPABILITY(true);
	bool tryLockShared() noexcept RB_TRY_ACQUIRE_SHARED_CAPABILITY(true);
	void unlock() RB_RELEASE_CAPABILITY();
	void unlockShared() RB_RELEASE_SHARED_CAPABILITY();

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

using SharedMutexLocker = LockGuard<SharedMutex>;

} // namespace rb::sync
