#include "Thread.hpp"

#include <rb/core/error/InvalidArgumentError.hpp>
#include <rb/sync/impl.hpp>

using namespace rb::core;
using namespace rb::sync;

#if RB_USE(PTHREADS)

Thread* Thread::current() noexcept {
}

Thread::Id Thread::currentId() noexcept {
}

void Thread::yield() noexcept {
}

Thread::Thread()
    : pImpl_(core::makeUnique<Impl>()) {
}

Thread::~Thread() {
}

Thread::Id Thread::id() const noexcept {
}

usize Thread::stackSize() const noexcept {
}

void Thread::detach() {
}

void Thread::exit(int exitCode) {
}

void Thread::join() {
}

void Thread::join(int& exitCode) {
}

void Thread::setStackSize(usize stackSize) {
}

#elif RB_USE(WIN32_THREADS)

	#include <process.h>

	#include <rb/sync/Mutex.hpp>

	#define RB_GUARD MutexLocker locker(pImpl_->mutex)

namespace {

int toRawPriority(Thread::Priority priority) noexcept {
	using Priority = Thread::Priority;
	switch (priority) {
		case Priority::kIdle        : return THREAD_PRIORITY_IDLE;
		case Priority::kLowest      : return THREAD_PRIORITY_LOWEST;
		case Priority::kLow         : return THREAD_PRIORITY_BELOW_NORMAL;
		case Priority::kNormal      : return THREAD_PRIORITY_NORMAL;
		case Priority::kHigh        : return THREAD_PRIORITY_ABOVE_NORMAL;
		case Priority::kHighest     : return THREAD_PRIORITY_HIGHEST;
		case Priority::kTimeCritical: return THREAD_PRIORITY_TIME_CRITICAL;
		case Priority::kInherit     : return GetThreadPriority(GetCurrentThread());
		default                     : RB_UNREACHABLE();
	}
}

thread_local Thread* thisThread;

} // namespace

// ReSharper disable CppMemberFunctionMayBeConst, CppMemberFunctionMayBeStatic

struct Thread::Impl {
	HANDLE impl = nullptr;
	StartFn fn = nullptr;
	void* arg = nullptr;
	usize stackSize RB_GUARDED_BY(mutex) = 0;
	unsigned id = 0;
	Priority priority RB_GUARDED_BY(mutex) = Priority::kInherit;

	Mutex mutex;
	bool finished RB_GUARDED_BY(mutex) = false;
	bool running RB_GUARDED_BY(mutex) = false;

	static unsigned WINAPI startThread(void* arg) {
		auto* thread = static_cast<Thread*>(arg);
		auto* impl = thread->pImpl_.operator->();

		thisThread = thread;

		int const exitCode = impl->fn(impl->arg);
		impl->finish();
		return exitCode;
	}

	void detach() {
		MutexLocker locker(mutex);
		if (!joinable()) {
			throw OsError(ErrorCode::kInvalidArgument)
			    .withMessage("Thread is not joinable");
		}

		reset();
		RB_SYNC_CHECK_LAST_ERROR(CloseHandle(impl));
	}

	void finish() RB_LOCKS_EXCLUDED(mutex) {
		MutexLocker locker(mutex);
		reset();
	}

	bool joinable() const noexcept RB_REQUIRES_CAPABILITY(mutex) {
		return running && !finished;
	}

	void reset() RB_REQUIRES_CAPABILITY(mutex) {
		priority = Priority::kInherit;
		finished = true;
		running = false;
		id = 0;
	}

	void setPriority(Priority priority) RB_REQUIRES_CAPABILITY(mutex) {
		this->priority = priority;
		RB_SYNC_CHECK_LAST_ERROR(SetThreadPriority(impl, toRawPriority(priority)));
	}

	void wait(DWORD milliseconds, int* exitCode = nullptr) RB_LOCKS_EXCLUDED(mutex) {
		MutexLocker locker(mutex);

		if (id == GetCurrentThreadId()) {
			throw OsError(ErrorCode::kResourceDeadlockWouldOccur)
			    .withMessage("Thread tried to wait on itself");
		}

		if (!joinable()) {
			throw OsError(ErrorCode::kInvalidArgument)
			    .withMessage("Thread is not joinable");
		}

		locker.lockable().unlock();
		OsError::RawCode rawCode = 0;
		if (WaitForSingleObject(impl, milliseconds)) {
			rawCode = GetLastError();
		}
		locker.lockable().lock();

		if (rawCode) {
			throw OsError::fromRawCode(rawCode);
		}

		if (exitCode) {
			DWORD code = 0;
			RB_SYNC_CHECK_LAST_ERROR(GetExitCodeThread(impl, &code));
			*exitCode = static_cast<int>(code);
		}

		reset();
		RB_SYNC_CHECK_LAST_ERROR(CloseHandle(impl));
	}
};

Thread* Thread::currentThread() noexcept {
	// TODO adopted thread
	return thisThread;
}

Thread::Id Thread::currentThreadId() noexcept {
	if (auto const* thread = currentThread()) {
		return thread->id();
	}
	return Id{};
}

void Thread::yield() noexcept {
	// https://stackoverflow.com/a/1383966/4884522
	Sleep(0);
}

Thread::Thread()
    : pImpl_(core::makeUnique<Impl>()) {
}

Thread::~Thread() noexcept(false) {
	if (joinable()) {
		throw OsError(ErrorCode::kOperationInProgress);
	}
}

bool Thread::joinable() const noexcept {
	RB_GUARD;
	return pImpl_->joinable();
}

Thread::Id Thread::id() const noexcept {
	return Id{pImpl_->id};
}

bool Thread::isFinished() const {
	RB_GUARD;
	return pImpl_->finished;
}

bool Thread::isRunning() const {
	RB_GUARD;
	return pImpl_->running;
}

Thread::Priority Thread::priority() const noexcept {
	RB_GUARD;
	return pImpl_->priority;
}

void Thread::setPriority(Priority priority) {
	if (priority == Priority::kInherit) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	RB_GUARD;
	if (!pImpl_->running) {
		throw OsError(ErrorCode::kOperationNotPermitted, ERROR_INVALID_HANDLE)
		    .withMessage("Cannot set priority, thread is not running");
	}

	pImpl_->setPriority(priority);
}

usize Thread::stackSize() const noexcept {
	RB_GUARD;
	return pImpl_->stackSize;
}

void Thread::setStackSize(usize stackSize) {
	RB_GUARD;
	if (pImpl_->running) {
		throw OsError(ErrorCode::kOperationNotPermitted)
		    .withMessage("Cannot change stack size while the thread is running");
	}

	pImpl_->stackSize = stackSize;
}

void Thread::detach() {
	pImpl_->detach();
}

void Thread::exit(int exitCode) { // NOLINT(*-convert-member-functions-to-static)
	_endthreadex(exitCode);
	RB_UNREACHABLE();
}

void Thread::join() {
	pImpl_->wait(INFINITE);
}

void Thread::join(int& exitCode) {
	pImpl_->wait(INFINITE, &exitCode);
}

void Thread::start(StartFn fn, void* arg, Priority priority) {
	if (!fn) {
		throw InvalidArgumentError("null thread function");
	}

	RB_GUARD;
	if (pImpl_->running) {
		return; // throw?
	}

	pImpl_->fn = fn;
	pImpl_->arg = arg;

	usize const handle = _beginthreadex(
	    nullptr,
	    pImpl_->stackSize,
	    Impl::startThread,
	    this,
	    CREATE_SUSPENDED,
	    &pImpl_->id);
	if (!handle) {
		// _beginthreadex uses errno instead of GetLastError()
		throw OsError::fromErrno();
	}

	pImpl_->impl = reinterpret_cast<HANDLE>(handle); // NOLINT(*-no-int-to-ptr,*-pro-type-reinterpret-cast)
	pImpl_->finished = false;
	pImpl_->running = true;
	pImpl_->setPriority(priority);
	RB_SYNC_CHECK_LAST_ERROR(ResumeThread(pImpl_->impl) != static_cast<DWORD>(-1));
}

#endif
