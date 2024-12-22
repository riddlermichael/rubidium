#include "Barrier.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

struct Barrier::Impl {
	pthread_barrier_t impl;
};

Barrier::Barrier(unsigned count)
    : pImpl_(core::makeUnique<Impl>()) {
	RB_SYNC_CHECK_ERRNO(pthread_barrier_init(RB_SYNC_IMPL, nullptr, count));
}

Barrier::~Barrier() noexcept(false) {
	RB_SYNC_CHECK_ERRNO(pthread_barrier_destroy(RB_SYNC_IMPL));
}

void Barrier::wait() noexcept(false) {
	int const error = pthread_barrier_wait(RB_SYNC_IMPL);
	if (error && error != PTHREAD_BARRIER_SERIAL_THREAD) {
		throw core::OsError::fromRawCode(error);
	}
}

#elif RB_USE(WIN32_THREADS)

struct Barrier::Impl {
	SYNCHRONIZATION_BARRIER impl;
};

Barrier::Barrier(unsigned count)
    : pImpl_(core::makeUnique<Impl>()) {
	/**
	 * BOOL InitializeSynchronizationBarrier(
	 * [out] LPSYNCHRONIZATION_BARRIER lpBarrier,
	 * [in]  LONG                      lTotalThreads,
	 * [in]  LONG                      lSpinCount
	 * );
	 *
	 * [in] lSpinCount
	 * The number of times an individual thread should spin while waiting for other threads to arrive at the barrier.
	 * If this parameter is -1, the thread spins 2000 times.
	 * If the thread exceeds lSpinCount, the thread blocks unless it called EnterSynchronizationBarrier
	 * with SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY.
	 */
	// NOLINTNEXTLINE(*-narrowing-conversions)
	RB_SYNC_CHECK_LAST_ERROR(InitializeSynchronizationBarrier(RB_SYNC_IMPL, count, -1));
}

Barrier::~Barrier() noexcept(true) {
	DeleteSynchronizationBarrier(RB_SYNC_IMPL); // always returns TRUE
}

void Barrier::wait() noexcept {
	// TRUE for the last thread to signal the barrier.
	// Threads that signal the barrier before the last thread signals it receive a return value of FALSE.
	EnterSynchronizationBarrier(RB_SYNC_IMPL, 0);
}

#endif
