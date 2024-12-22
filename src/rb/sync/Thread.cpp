#include "Thread.hpp"

#include <rb/core/error/InvalidArgumentError.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/IsIntegral.hpp>
#include <rb/core/traits/IsPointer.hpp>
#include <rb/sync/impl.hpp>
#include <rb/sync/Mutex.hpp>

using namespace rb::core;
using namespace rb::sync;

#define RB_GUARD MutexLocker locker(pImpl_->mutex)

namespace {

thread_local Thread* thisThread;

} // namespace

#if RB_USE(PTHREADS)

namespace {
static_assert(sizeof(pthread_t) <= sizeof(usize));

	#if !defined(RB_OS_ANDROID)                             \
	        && !defined(RB_OS_OPENBSD)                      \
	        && defined(_POSIX_THREAD_PRIORITY_SCHEDULING)   \
	        && (_POSIX_THREAD_PRIORITY_SCHEDULING - 0 >= 0) \
	    || defined(RB_OS_DARWIN)
		#define RB_HAS_THREAD_PRIORITY_SCHEDULING 1
	#else
		#define RB_HAS_THREAD_PRIORITY_SCHEDULING (-1)
	#endif

	#if defined(RB_OS_LINUX) && !defined(SCHED_IDLE)
		#define SCHED_IDLE 5
	#endif

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

template <class T>
constexpr auto toUsize(T value) noexcept
    -> RB_REQUIRES_RETURN(usize, isIntegral<T>) {
	return static_cast<usize>(value);
}

template <class T>
constexpr auto toUsize(T value) noexcept
    -> RB_REQUIRES_RETURN(usize, isPointer<T>) {
	return reinterpret_cast<usize>(value); // NOLINT(*-pro-type-reinterpret-cast)
}

} // namespace

struct Thread::Impl {
	pthread_t impl = {};
	// StartFn fn = nullptr;
	// void* arg = nullptr;
	usize stackSize RB_GUARDED_BY(mutex) = 0;
	Priority priority RB_GUARDED_BY(mutex) = Priority::kInherit;

	Mutex mutable mutex;
	bool finished RB_GUARDED_BY(mutex) = false;
	bool running RB_GUARDED_BY(mutex) = false;

	static void* startThread(void* arg) {
		RB_UNUSED(arg);
		return nullptr;
	}

	bool joinable() const noexcept RB_REQUIRES_CAPABILITY(mutex) {
		return running && !finished;
	}

	void setPriority(Priority priority) RB_REQUIRES_CAPABILITY(mutex) {
		this->priority = priority;
	#if RB_HAS(THREAD_PRIORITY_SCHEDULING) // NOLINT(*-redundant-preprocessor)
		int policy = 0;
		sched_param param = {};
		if (pthread_getschedparam(impl, &policy, &param)) {
			return;
		}

		int prio = 0;
		if (!getRawPriority(priority, policy, prio)) {
			return;
		}

		param.sched_priority = prio;
		int status = pthread_setschedparam(impl, policy, &param);
		#ifdef SCHED_IDLE
		if (status == -1 && policy == SCHED_IDLE && errno == EINVAL) {
			pthread_getschedparam(impl, &policy, &param);
			param.sched_priority = sched_get_priority_min(policy);
			pthread_setschedparam(impl, policy, &param);
		}
		#else
		RB_UNUSED(status);
		#endif
	#endif
	}
};

void Thread::yield() noexcept {
	sched_yield();
}

Thread::Id Thread::id() const noexcept {
	return Id{toUsize(pImpl_->impl)};
}

void Thread::detach() {
}

void Thread::exit(int exitCode) {
	RB_UNUSED(exitCode);
	RB_UNREACHABLE();
}

void Thread::join() {
}

void Thread::join(int& exitCode) {
	RB_UNUSED(exitCode);
}

void Thread::start(StartFn fn, void* arg, Priority priority) {
	RB_UNUSED(arg);
	if (!fn) {
		throw InvalidArgumentError("Null thread function");
	}

	RB_GUARD;
	if (pImpl_->running) {
		return; // throw?
	}

	// pImpl_->fn = fn;
	// pImpl_->arg = arg;

	pthread_attr_t attr{};
	AttributeGuard _(attr);
	pImpl_->priority = priority; // FIXME

	if (pImpl_->stackSize > 0) {
	#if defined(_POSIX_THREAD_ATTR_STACKSIZE) && (_POSIX_THREAD_ATTR_STACKSIZE - 0 > 0)
		int const code = pthread_attr_setstacksize(&attr, pImpl_->stackSize);
	#else
		constexpr int code = ENOSYS;
	#endif
		// ReSharper disable once CppCompileTimeConstantCanBeReplacedWithBooleanConstant
		if (code) {
			throw OsError::fromErrno(code);
		}
	}

	pImpl_->running = true;
}

#elif RB_USE(WIN32_THREADS)

	#include <process.h>

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

} // namespace

// ReSharper disable CppMemberFunctionMayBeConst, CppMemberFunctionMayBeStatic

struct Thread::Impl {
	HANDLE impl = nullptr;
	StartFn fn = nullptr;
	void* arg = nullptr;
	usize stackSize RB_GUARDED_BY(mutex) = 0;
	unsigned id = 0;
	Priority priority RB_GUARDED_BY(mutex) = Priority::kInherit;

	Mutex mutable mutex;
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

void Thread::yield() noexcept {
	// https://stackoverflow.com/a/1383966/4884522
	Sleep(0);
}

Thread::Id Thread::id() const noexcept {
	return Id{pImpl_->id};
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
		throw InvalidArgumentError("Null thread function");
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

Thread* Thread::currentThread() noexcept {
	return thisThread;
}

Thread::Id Thread::currentThreadId() noexcept {
	if (auto const* thread = currentThread()) {
		return thread->id();
	}
	return Id{};
}

Thread::Thread()
    : pImpl_(core::makeUnique<Impl>()) {
}

Thread::~Thread() noexcept(false) {
	if (joinable()) {
		throw OsError(ErrorCode::kOperationInProgress);
	}
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
		throw OsError(ErrorCode::kOperationNotPermitted
#if RB_USE(WIN32_THREADS)
		    ,
		    ERROR_INVALID_HANDLE
#endif
		    )
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

bool Thread::isFinished() const {
	RB_GUARD;
	return pImpl_->finished;
}

bool Thread::isRunning() const {
	RB_GUARD;
	return pImpl_->running;
}

bool Thread::joinable() const noexcept {
	RB_GUARD;
	return pImpl_->joinable();
}
