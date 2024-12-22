#include "ConditionVariable.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

struct ConditionVariable::Impl {
	pthread_cond_t impl;
};

ConditionVariable::ConditionVariable()
    : pImpl_(core::makeUnique<Impl>()) {
	RB_SYNC_CHECK_ERRNO(pthread_cond_init(RB_SYNC_IMPL, nullptr));
}

ConditionVariable::~ConditionVariable() noexcept(false) {
	RB_SYNC_CHECK_ERRNO(pthread_cond_destroy(RB_SYNC_IMPL));
}

void ConditionVariable::notify() {
	RB_SYNC_CHECK_ERRNO(pthread_cond_signal(RB_SYNC_IMPL));
}

void ConditionVariable::notifyAll() {
	RB_SYNC_CHECK_ERRNO(pthread_cond_broadcast(RB_SYNC_IMPL));
}

void ConditionVariable::wait(Mutex& mutex) {
	auto* mu = static_cast<pthread_mutex_t*>(mutex.rawImpl());
	RB_SYNC_CHECK_ERRNO(pthread_cond_wait(RB_SYNC_IMPL, mu));
}

#elif RB_USE(WIN32_THREADS)

struct ConditionVariable::Impl {
	CONDITION_VARIABLE impl;
};

ConditionVariable::ConditionVariable()
    : pImpl_(core::makeUnique<Impl>()) {
	InitializeConditionVariable(RB_SYNC_IMPL);
}

// https://joeduffyblog.com/2006/11/28/windows-keyed-events-critical-sections-and-new-vista-synchronization-features/
ConditionVariable::~ConditionVariable() noexcept(true) = default;

void ConditionVariable::notify() {
	WakeConditionVariable(RB_SYNC_IMPL);
}

void ConditionVariable::notifyAll() {
	WakeAllConditionVariable(RB_SYNC_IMPL);
}

void ConditionVariable::wait(Mutex& mutex) {
	auto* cs = static_cast<CRITICAL_SECTION*>(mutex.rawImpl());
	RB_SYNC_CHECK_LAST_ERROR(SleepConditionVariableCS(RB_SYNC_IMPL, cs, INFINITE));
}

#endif
