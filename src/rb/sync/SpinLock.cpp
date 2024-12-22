#include "SpinLock.hpp"

#include <rb/sync/impl.hpp>

#include <rb/core/error/NotImplementedError.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

struct SpinLock::Impl {
	pthread_spinlock_t impl;
};

SpinLock::SpinLock()
    : pImpl_(core::makeUnique<Impl>()) {
	RB_SYNC_CHECK(pthread_spin_init(RB_SYNC_IMPL, PTHREAD_PROCESS_PRIVATE));
}

SpinLock::~SpinLock() noexcept(false) {
	RB_SYNC_CHECK(pthread_spin_destroy(RB_SYNC_IMPL));
}

void SpinLock::lock() {
	RB_SYNC_CHECK(pthread_spin_lock(RB_SYNC_IMPL));
}

bool SpinLock::tryLock() noexcept {
	return !pthread_spin_trylock(RB_SYNC_IMPL);
}

void SpinLock::unlock() {
	RB_SYNC_CHECK(pthread_spin_unlock(RB_SYNC_IMPL));
}

#elif RB_USE(WIN32_THREADS)

#endif
