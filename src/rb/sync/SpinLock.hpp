#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/sync/LockGuard.hpp>

namespace rb::sync {

class RB_EXPORT RB_CAPABILITY("rb::sync::SpinLock") SpinLock final {
public:
	SpinLock();
	~SpinLock() noexcept(false);

	void lock() RB_ACQUIRE_CAPABILITY();
	bool tryLock() noexcept RB_TRY_ACQUIRE_CAPABILITY(true);
	void unlock() RB_RELEASE_CAPABILITY();

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

using SpinLocker = LockGuard<SpinLock>;

} // namespace rb::sync
