#include "Semaphore.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

	#include <semaphore.h>

struct Semaphore::Impl {
	sem_t impl;
};

Semaphore::Semaphore(unsigned count)
    : pImpl_(core::makeUnique<Impl>()) {
	if (sem_init(RB_SYNC_IMPL, false, count)) {
		throw core::OsError::fromErrno();
	}
}

Semaphore::~Semaphore() noexcept(false) {
	if (sem_destroy(RB_SYNC_IMPL)) {
		throw core::OsError::fromErrno();
	}
}

void Semaphore::acquire() {
	if (sem_wait(RB_SYNC_IMPL)) {
		throw core::OsError::fromErrno();
	}
}

bool Semaphore::tryAcquire() noexcept {
	return !sem_trywait(RB_SYNC_IMPL);
}

void Semaphore::release() {
	if (sem_post(RB_SYNC_IMPL)) {
		throw core::OsError::fromErrno();
	}
}

#elif RB_USE(WIN32_THREADS)

struct Semaphore::Impl {
	HANDLE impl;
};

Semaphore::Semaphore(unsigned count)
    : pImpl_(core::makeUnique<Impl>()) {
	pImpl_->impl = CreateSemaphoreA(nullptr, count, count, nullptr);
	if (!pImpl_->impl) {
		throw core::OsError::lastOsError();
	}
}

Semaphore::~Semaphore() noexcept(false) {
	RB_SYNC_CHECK_LAST_ERROR(CloseHandle(pImpl_->impl));
}

void Semaphore::acquire() {
	DWORD const rc = WaitForSingleObject(pImpl_->impl, INFINITE);
	switch (rc) {
		case WAIT_ABANDONED: throw core::OsError::fromRawCode(ERROR_SEM_OWNER_DIED);
		case WAIT_FAILED   : throw core::OsError::lastOsError();
		case WAIT_OBJECT_0 : return;
		case WAIT_TIMEOUT  : throw core::OsError::fromRawCode(ERROR_SEM_TIMEOUT);
		default            : RB_UNREACHABLE();
	}
}

bool Semaphore::tryAcquire() noexcept {
	return !WaitForSingleObject(pImpl_->impl, 0);
}

void Semaphore::release() {
	RB_SYNC_CHECK_LAST_ERROR(ReleaseSemaphore(pImpl_->impl, 1, nullptr));
}

#endif
