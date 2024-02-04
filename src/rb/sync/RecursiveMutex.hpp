#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/sync.hpp>
#include <rb/sync/LockGuard.hpp>

namespace rb::sync {

class RB_EXPORT RB_CAPABILITY("rb::sync::RecursiveMutex") RecursiveMutex final {
	friend class ConditionVariable;

public:
	RecursiveMutex();
	~RecursiveMutex() noexcept(RB_USE(WIN32_THREADS));

	RB_DISABLE_COPY(RecursiveMutex);

	void lock();
	bool tryLock() noexcept;
	void unlock() noexcept(RB_USE(WIN32_THREADS));

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

using RecursiveMutexLocker = LockGuard<RecursiveMutex>;

} // namespace rb::sync
