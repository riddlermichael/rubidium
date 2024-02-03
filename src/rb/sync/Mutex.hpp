#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/sync.hpp>
#include <rb/sync/LockGuard.hpp>

namespace rb::sync {

class RB_EXPORT RB_CAPABILITY("rb::sync::Mutex") Mutex final {
	friend class ConditionVariable;

public:
	Mutex();
	~Mutex() noexcept(RB_USE(WIN32_THREADS));

	RB_DISABLE_COPY(Mutex);

	void lock() RB_ACQUIRE_CAPABILITY();
	bool tryLock() noexcept RB_TRY_ACQUIRE_CAPABILITY(true);
	void unlock() noexcept(RB_USE(WIN32_THREADS)) RB_RELEASE_CAPABILITY();

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

using MutexLocker = LockGuard<Mutex>;

} // namespace rb::sync
