#pragma once

#include <rb/sync/ConditionVariable.hpp>

namespace rb::sync {

class RB_EXPORT RB_CAPABILITY("rb::sync::ConditionMutex") ConditionMutex final {
public:
	void lock() RB_ACQUIRE_CAPABILITY();
	bool tryLock() noexcept RB_TRY_ACQUIRE_CAPABILITY(true);
	void unlock() RB_RELEASE_CAPABILITY();

	template <class Condition>
	void await(Condition&& condition) {
		if (RB_FWD(condition)()) {
			return;
		}

		cv_.notifyAll();
		do { // NOLINT(*-avoid-do-while)
			cv_.notifyAll();
			cv_.wait(mu_);
		} while (!RB_FWD(condition)());
	}

	template <class Condition>
	void lockWhen(Condition&& condition) RB_ACQUIRE_CAPABILITY() {
		lock();
		await(RB_FWD(condition));
	}

private:
	Mutex mu_;
	CondVar cv_;
};

using ConditionMutexLocker = LockGuard<ConditionMutex>;

} // namespace rb::sync
