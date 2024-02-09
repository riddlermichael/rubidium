#include "RecursiveMutex.hpp"

#include <rb/sync/impl.hpp>

using namespace rb::sync;

#if RB_USE(PTHREADS)

struct RecursiveMutex::Impl {
	pthread_mutex_t impl;
};

namespace {

struct AttributeGuard {
	pthread_mutexattr_t& attr;

	explicit AttributeGuard(pthread_mutexattr_t& attr)
	    : attr(attr) {
		RB_SYNC_CHECK(pthread_mutexattr_init(&attr));
	}

	~AttributeGuard() noexcept(false) {
		RB_SYNC_CHECK(pthread_mutexattr_destroy(&attr));
	}
};

} // namespace

RecursiveMutex::RecursiveMutex()
    : pImpl_(core::makeUnique<Impl>()) {
	pthread_mutexattr_t attr{};
	AttributeGuard _(attr);
	RB_SYNC_CHECK(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
	#if !defined(RB_COMPILER_MINGW) && !defined(RB_OS_CYGWIN)
	RB_SYNC_CHECK(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
	#endif
	RB_SYNC_CHECK(pthread_mutex_init(RB_SYNC_IMPL, &attr));
}

RecursiveMutex::~RecursiveMutex() noexcept(false) {
	RB_SYNC_CHECK(pthread_mutex_destroy(RB_SYNC_IMPL));
}

void RecursiveMutex::lock() {
	RB_SYNC_CHECK(pthread_mutex_lock(RB_SYNC_IMPL));
}

bool RecursiveMutex::tryLock() noexcept {
	return !pthread_mutex_trylock(RB_SYNC_IMPL);
}

void RecursiveMutex::unlock() noexcept(false) {
	RB_SYNC_CHECK(pthread_mutex_unlock(RB_SYNC_IMPL));
}

#elif RB_USE(WIN32_THREADS)

struct RecursiveMutex::Impl {
	CRITICAL_SECTION impl;
};

namespace {

// error C2712: cannot use __try in functions that require object unwinding fix

bool initCS(CRITICAL_SECTION* cs) noexcept {
	/**
	 * Windows Server 2003 and Windows XP:
	 * In low memory situations, InitializeCriticalSection can raise a STATUS_NO_MEMORY exception.
	 * Starting with Windows Vista, this exception was eliminated and InitializeCriticalSection always succeeds,
	 * even in low memory situations.
	 */
	#if _WIN32_WINNT >= _WIN32_WINNT_VISTA // NOLINT(*-redundant-preprocessor)
	InitializeCriticalSection(cs);
	#else
	__try {
		InitializeCriticalSection(cs);
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
	#endif
	return true;
}

bool lockImpl(CRITICAL_SECTION* cs) noexcept {
	/**
	 * This function can raise EXCEPTION_POSSIBLE_DEADLOCK, also known as STATUS_POSSIBLE_DEADLOCK,
	 * if a wait operation on the critical section times out.
	 * The timeout interval is specified by the following registry value:
	 * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\CriticalSectionTimeout.
	 */
	__try {
		EnterCriticalSection(cs);
		return true;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

} // namespace

RecursiveMutex::RecursiveMutex()
    : pImpl_(core::makeUnique<Impl>()) {
	// ReSharper disable once CppDFAConstantConditions, CppDFAUnreachableCode
	if (!initCS(RB_SYNC_IMPL)) {
		throw core::OsError::fromRawCode(ERROR_NOT_ENOUGH_MEMORY);
	}
}

RecursiveMutex::~RecursiveMutex() noexcept(true) {
	DeleteCriticalSection(RB_SYNC_IMPL);
}

void RecursiveMutex::lock() {
	if (!lockImpl(RB_SYNC_IMPL)) {
		throw core::OsError::fromRawCode(ERROR_POSSIBLE_DEADLOCK);
	}
}

bool RecursiveMutex::tryLock() noexcept {
	return TryEnterCriticalSection(RB_SYNC_IMPL);
}

void RecursiveMutex::unlock() noexcept {
	LeaveCriticalSection(RB_SYNC_IMPL);
}

#endif
