#include "SpinLock.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

	#ifdef _POSIX_SPIN_LOCKS

struct SpinLock::Impl {
	pthread_spinlock_t impl;
};

SpinLock::SpinLock()
    : pImpl_(core::makeUnique<Impl>()) {
	RB_SYNC_CHECK_ERRNO(pthread_spin_init(RB_SYNC_IMPL, PTHREAD_PROCESS_PRIVATE));
}

SpinLock::~SpinLock() noexcept(false) {
	RB_SYNC_CHECK_ERRNO(pthread_spin_destroy(RB_SYNC_IMPL));
}

void SpinLock::lock() {
	RB_SYNC_CHECK_ERRNO(pthread_spin_lock(RB_SYNC_IMPL));
}

bool SpinLock::tryLock() noexcept {
	return !pthread_spin_trylock(RB_SYNC_IMPL);
}

void SpinLock::unlock() {
	RB_SYNC_CHECK_ERRNO(pthread_spin_unlock(RB_SYNC_IMPL));
}

	#endif

#elif RB_USE(WIN32_THREADS)

#endif
