#include "ConditionMutex.hpp"

using namespace rb::sync;

void ConditionMutex::lock() {
	mu_.lock();
}

bool ConditionMutex::tryLock() noexcept {
	return mu_.tryLock();
}

void ConditionMutex::unlock() {
	cv_.notifyAll();
	mu_.unlock();
}
