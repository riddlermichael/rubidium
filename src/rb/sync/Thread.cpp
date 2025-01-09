#include "Thread.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::core;
using namespace rb::sync;

#if RB_USE(PTHREADS)

namespace {

struct AttributeGuard {
	pthread_attr_t& attr;

	explicit AttributeGuard(pthread_attr_t& attr)
	    : attr(attr) {
		RB_SYNC_CHECK_ERRNO(pthread_attr_init(&attr));
	}

	~AttributeGuard() noexcept(false) {
		RB_SYNC_CHECK_ERRNO(pthread_attr_destroy(&attr));
	}
};

void* threadFunc(void* arg) {
	auto* thread = static_cast<Thread*>(arg);
	thread->run();
	return nullptr;
}

} // namespace

struct Thread::Impl {
	pthread_t impl;
};

void Thread::join() {
	if (!pImpl_) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	RB_SYNC_CHECK_ERRNO(pthread_join(pImpl_->impl, nullptr));
}

void Thread::start() {
	pthread_attr_t attr{};
	AttributeGuard const _{attr};
	RB_SYNC_CHECK_ERRNO(pthread_attr_init(&attr));
	RB_SYNC_CHECK_ERRNO(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	RB_SYNC_CHECK_ERRNO(pthread_create(RB_SYNC_IMPL, &attr, threadFunc, this));
	RB_SYNC_CHECK_ERRNO(pthread_attr_destroy(&attr));
}

#elif RB_USE(WIN32_THREADS)

	#include <process.h>

struct Thread::Impl {
	HANDLE hThread = nullptr;
	unsigned id = 0;
};

namespace {

unsigned WINAPI threadFunc(void* arg) noexcept {
	auto* thread = static_cast<Thread*>(arg);
	thread->run();
	return 0;
}

} // namespace

Thread::Id Thread::currentThreadId() noexcept {
	return Id{GetCurrentThreadId()};
}

void Thread::sleepFor(time::Duration timeout) noexcept {
	if (timeout.isNegative()) {
		return;
	}

	auto const ms = timeout.toMilliseconds();
	if (ms >= INFINITE) {
		return;
	}

	Sleep(static_cast<DWORD>(ms));
}

void Thread::yield() noexcept {
	// https://stackoverflow.com/a/1383966/4884522
	Sleep(0);
}

Thread::Thread()
    : pImpl_(makeUnique<Impl>()) {
	pImpl_->hThread = (HANDLE) _beginthreadex( // NOLINT
	    nullptr,
	    0, // TODO
	    threadFunc,
	    this,
	    CREATE_SUSPENDED,
	    &pImpl_->id);
	RB_SYNC_CHECK_ERRNO(errno);
}

Thread::~Thread() noexcept(false) {
	if (joinable()) {
		throw OsError(ErrorCode::kOperationInProgress);
	}
}

void Thread::swap(Thread& rhs) noexcept {
	pImpl_.swap(rhs.pImpl_);
}

Thread::Id Thread::id() const noexcept {
	return Id{pImpl_->id};
}

bool Thread::joinable() const noexcept {
	return pImpl_->id;
}

void Thread::detach() {
	if (!joinable()) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	RB_SYNC_CHECK_LAST_ERROR(CloseHandle(pImpl_->hThread));
	pImpl_ = makeUnique<Impl>();
}

void Thread::join() {
	if (!joinable()) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	if (pImpl_->id == GetCurrentThreadId()) {
		throw OsError(ErrorCode::kResourceDeadlockWouldOccur);
	}

	DWORD const code = WaitForSingleObject(pImpl_->hThread, INFINITE);
	if (code != WAIT_OBJECT_0) {
		throw OsError::lastOsError();
	}

	pImpl_ = makeUnique<Impl>();
}

void Thread::start(SourceLocation const& location) {
	DWORD const suspendCount = ResumeThread(pImpl_->hThread);
	if (suspendCount == 0) {
		throw OsError(ErrorCode::kOperationInProgress, location);
	}
	if (suspendCount == -1) {
		throw OsError::lastOsError(location);
	}
}

#endif
