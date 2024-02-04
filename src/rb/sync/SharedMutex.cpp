#include "SharedMutex.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

namespace {

struct AttributeGuard {
	pthread_rwlockattr_t& attr;

	explicit AttributeGuard(pthread_rwlockattr_t& attr)
	    : attr(attr) {
		RB_SYNC_CHECK(pthread_rwlockattr_init(&attr));
	}

	~AttributeGuard() noexcept(false) {
		RB_SYNC_CHECK(pthread_rwlockattr_destroy(&attr));
	}
};

} // namespace

struct SharedMutex::Impl {
	pthread_rwlock_t impl;
};

SharedMutex::SharedMutex(Policy policy)
    : pImpl_(core::makeUnique<Impl>()) {
	pthread_rwlockattr_t attr{};
	AttributeGuard _(attr);

	#ifdef RB_OS_WIN
	RB_UNUSED(policy);
	#else
	RB_SYNC_CHECK(pthread_rwlockattr_setkind_np(&attr, policy));
	#endif

	RB_SYNC_CHECK(pthread_rwlock_init(RB_SYNC_IMPL, &attr));
}

SharedMutex::~SharedMutex() noexcept(false) {
	RB_SYNC_CHECK(pthread_rwlock_destroy(RB_SYNC_IMPL));
}

void SharedMutex::lock() {
	RB_SYNC_CHECK(pthread_rwlock_wrlock(RB_SYNC_IMPL));
}

void SharedMutex::lockShared() {
	RB_SYNC_CHECK(pthread_rwlock_rdlock(RB_SYNC_IMPL));
}

bool SharedMutex::tryLock() noexcept {
	return !pthread_rwlock_trywrlock(RB_SYNC_IMPL);
}

bool SharedMutex::tryLockShared() noexcept {
	return !pthread_rwlock_tryrdlock(RB_SYNC_IMPL);
}

void SharedMutex::unlock() {
	RB_SYNC_CHECK(pthread_rwlock_unlock(RB_SYNC_IMPL));
}

void SharedMutex::unlockShared() {
	RB_SYNC_CHECK(pthread_rwlock_unlock(RB_SYNC_IMPL));
}

#elif RB_USE(WIN32_THREADS)

struct SharedMutex::Impl {
	SRWLOCK impl;
};

SharedMutex::SharedMutex(Policy policy)
    : pImpl_(core::makeUnique<Impl>()) {
	RB_UNUSED(policy);
	InitializeSRWLock(RB_SYNC_IMPL);
}

// an unlocked SRW lock with no waiting threads is in its initial state and can be copied, moved,
// and forgotten without being explicitly destroyed
SharedMutex::~SharedMutex() = default;

void SharedMutex::lock() {
	AcquireSRWLockExclusive(RB_SYNC_IMPL);
}

void SharedMutex::lockShared() {
	AcquireSRWLockShared(RB_SYNC_IMPL);
}

bool SharedMutex::tryLock() noexcept {
	return TryAcquireSRWLockExclusive(RB_SYNC_IMPL);
}

bool SharedMutex::tryLockShared() noexcept {
	return TryAcquireSRWLockShared(RB_SYNC_IMPL);
}

void SharedMutex::unlock() {
	ReleaseSRWLockExclusive(RB_SYNC_IMPL);
}

void SharedMutex::unlockShared() {
	ReleaseSRWLockShared(RB_SYNC_IMPL);
}

#endif
