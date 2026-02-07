#include "Thread.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <cstring>

#include <rb/sync/impl.hpp>

using namespace rb::core;
using namespace rb::sync;

namespace {
thread_local Thread* thisThread = nullptr;
} // namespace

#if RB_USE(PTHREADS)

	#ifndef RBC_COMPILER_MINGW
		#include <sched.h>
	#endif

namespace {

static_assert(sizeof(pthread_t) <= sizeof(usize));

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

void* threadFunc(void* arg) noexcept {
	auto* thread = static_cast<Thread*>(arg);
	thisThread = thread;
	thread->run();
	return nullptr;
}

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

template <class T>
auto toUsize(T value) noexcept
    -> RB_REQUIRES_RETURN(usize, !isIntegral<T> && !isPointer<T>) {
	static_assert(sizeof(T) <= sizeof(usize));
	u8 data[sizeof(usize)] = {};
	std::memcpy(data, &value, sizeof(value));
	return bitCast<usize>(data);
}

} // namespace

struct Thread::Impl {
	pthread_t impl = {};
};

Thread::Id Thread::currentThreadId() noexcept {
	return Id{toUsize(pthread_self())};
}

void Thread::sleepFor(time::Duration timeout) noexcept {
	// ReSharper disable CppRedundantCastExpression
	if (RB_UNLIKELY(timeout.isNaN())
	    || timeout.isNegative()) {
		return;
	}

	if (timeout.isInf()) {
		timeout = time::Duration::max();
	}

	auto [secs, nsecs] = *timeout.toTimeSpec(); // NOLINT(*-cplusplus.Move)
	while (secs > 0 || nsecs > 0) {
		constexpr auto kMaxTimeT = static_cast<i64>(max<std::time_t>);
		std::timespec ts = {
		    static_cast<std::time_t>(kMaxTimeT < secs ? kMaxTimeT : secs),
		    static_cast<long>(nsecs),
		};
		secs -= static_cast<i64>(ts.tv_sec);
		if (nanosleep(&ts, &ts) == -1) {
			secs += static_cast<i64>(ts.tv_sec);
			nsecs = ts.tv_nsec;
		} else {
			nsecs = 0;
		}
	}
}

void Thread::yield() noexcept {
	sched_yield();
}

Thread::Thread()
    : pImpl_(makeUnique<Impl>()) {
}

Thread::Id Thread::id() const noexcept {
	return Id{toUsize(pImpl_->impl)};
}

bool Thread::joinable() const noexcept {
	return id() != Id{};
}

void Thread::detach() {
	if (!joinable()) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	RB_SYNC_CHECK_ERRNO(pthread_detach(pImpl_->impl));
	pImpl_ = makeUnique<Impl>();
}

void Thread::join() {
	if (!joinable()) {
		throw OsError(ErrorCode::kInvalidArgument);
	}

	RB_SYNC_CHECK_ERRNO(pthread_join(pImpl_->impl, nullptr));
	pImpl_ = makeUnique<Impl>();
}

void Thread::start() {
	if (joinable()) {
		throw OsError(ErrorCode::kOperationInProgress);
	}

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
	thisThread = thread;
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

	// TODO use waitable timer for duration < 1ms
	SleepEx(static_cast<DWORD>(ms), FALSE);
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

void Thread::start() {
	DWORD const suspendCount = ResumeThread(pImpl_->hThread);
	if (suspendCount == 0) {
		throw OsError(ErrorCode::kOperationInProgress);
	}
	if (suspendCount == -1) {
		throw OsError::lastOsError();
	}
}

#endif

Thread* Thread::currentThread() noexcept {
	return thisThread;
}

Thread::~Thread() noexcept(false) {
	if (joinable()) {
		throw OsError(ErrorCode::kOperationInProgress);
	}
}

void Thread::swap(Thread& rhs) noexcept {
	pImpl_.swap(rhs.pImpl_);
}

void Thread::sleepUntil(time::Instant instant) noexcept {
	auto const duration = instant.since(time::Instant::now());
	if (duration.isPositive()) {
		sleepFor(duration);
	}
}
