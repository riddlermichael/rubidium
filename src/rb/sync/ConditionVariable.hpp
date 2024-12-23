#pragma once

#include <rb/sync/Mutex.hpp>

namespace rb::sync {

class RB_EXPORT ConditionVariable final {
public:
	ConditionVariable();
	~ConditionVariable() noexcept(RB_USE(WIN32_THREADS));

	void notify();
	void notifyAll();
	void wait(Mutex& mutex) RB_REQUIRES_CAPABILITY(mutex);

	void wait(MutexLocker& locker) RB_REQUIRES_CAPABILITY(locker.lockable()) {
		wait(locker.lockable());
	}

	template <class Condition>
	void wait(MutexLocker& mutexLocker, Condition condition) {
		while (!RB_FWD(condition)()) {
			wait(mutexLocker);
		}
	}

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

using CondVar = ConditionVariable;

} // namespace rb::sync
