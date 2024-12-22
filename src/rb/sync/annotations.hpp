#pragma once

#include <rb/core/compiler.hpp>

/** @file

<b>All macros in this file work in clang ONLY</b>.

@code
class RB_CAPABILITY("mutex") Mutex {
public:
	// Acquire/lock this mutex exclusively. Only one thread can have exclusive access at any one time.
	// Write operations to guarded data require an exclusive lock.
	void lock() RB_ACQUIRE_CAPABILITY();

	// Acquire/lock this mutex for read operations, which require only a shared lock.
	// This assumes a multiple-reader, single writer semantics.
	// Multiple threads may acquire the mutex simultaneously as readers,
	// but a writer must wait for all of them to release the mutex before it can acquire it exclusively.
	void readerLock() RB_ACQUIRE_SHARED_CAPABILITY();

	// Release/unlock an exclusive mutex.
	void unlock() RB_RELEASE_CAPABILITY();

	// Release/unlock a shared mutex.
	void readerUnlock() RB_RELEASE_SHARED_CAPABILITY();

	// Generic unlock, can unlock exclusive and shared mutexes.
	void genericUnlock() RB_RELEASE_GENERIC_CAPABILITY();

	// Try to acquire the mutex. Returns true on success, and false on failure.
	bool tryLock() RB_TRY_ACQUIRE_CAPABILITY(true);

	// Try to acquire the mutex for read operations.
	bool readerTryLock() RB_TRY_ACQUIRE_SHARED_CAPABILITY(true);

	// Assert that this mutex is currently held by the calling thread.
	void assertHeld() RB_ASSERT_CAPABILITY(this);

	// Assert that is mutex is currently held for read operations.
	void assertReaderHeld() RB_ASSERT_SHARED_CAPABILITY(this);

	// For negative capabilities.
	Mutex const& operator!() const {
		return *this;
	}
};
@endcode

@see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
*/

/** @def RB_GUARDED_BY(x)
`RB_GUARDED_BY` is an attribute on data members,
which declares that the data member is protected by the given capability @p x.
Read operations on the data require shared access, while write operations require exclusive access.
@code
Mutex mu;
int *p1             RB_GUARDED_BY(mu);
int *p2             RB_PT_GUARDED_BY(mu);
unique_ptr<int> p3  RB_PT_GUARDED_BY(mu);

void test() {
    p1 = 0;             // Warning!

    *p2 = 42;           // Warning!
  	p2 = new int;       // OK.

    *p3 = 42;           // Warning!
    p3.reset(new int);	// OK.
}
@endcode

@def RB_PT_GUARDED_BY(x)
`RB_PT_GUARDED_BY` is similar to `RB_GUARDED_BY`, but is intended for use on pointers and smart pointers.
There is no constraint on the data member itself,
but the <em>data that it points</em> to is protected by the given capability.
@see RB_GUARDED_BY

@def RB_REQUIRES_CAPABILITY(...)
`RB_REQUIRES_CAPABILITY` is an attribute on functions or methods, which declares that the calling thread
must have exclusive access to the given capabilities.
More than one capability may be specified.
The capabilities must be held on entry to the function, <em>and must still be held on exit</em>.
@code
Mutex mu1
Mutex mu2;
int a RB_GUARDED_BY(mu1);
int b RB_GUARDED_BY(mu2);

void foo() RB_REQUIRES_CAPABILITY(mu1, mu2) {
    a = 0;
    b = 0;
}

void test() {
    mu1.lock();
    foo();         // Warning!  Requires mu2.
    mu1.unlock();
}
@endcode

@def RB_REQUIRES_SHARED_CAPABILITY(...)
`RB_REQUIRES_SHARED_CAPABILITY` is similar to `RB_REQUIRES_CAPABILITY`, but requires only shared access.
@see RB_REQUIRES_CAPABILITY
*/

#if defined(RB_COMPILER_CLANG)
	#define RB_CAPABILITY(name) __attribute__((capability(name)))
	#define RB_LOCKABLE RB_CAPABILITY
	#define RB_SCOPED_CAPABILITY __attribute__((scoped_lockable))
	#define RB_SCOPED_LOCKABLE RB_SCOPED_CAPABILITY
	#define RB_GUARDED_BY(x) __attribute__((guarded_by(x)))
	#define RB_PT_GUARDED_BY(x) __attribute__((pt_guarded_by(x)))
	#define RB_ACQUIRED_BEFORE(...) __attribute__((acquired_before(__VA_ARGS__)))
	#define RB_ACQUIRED_AFTER(...) __attribute__((acquired_after(__VA_ARGS__)))
	#define RB_REQUIRES_CAPABILITY(...) __attribute__((requires_capability(__VA_ARGS__)))
	#define RB_REQUIRES_SHARED_CAPABILITY(...) __attribute__((requires_shared_capability(__VA_ARGS__)))
	#define RB_ACQUIRE_CAPABILITY(...) __attribute__((acquire_capability(__VA_ARGS__)))
	#define RB_ACQUIRE_SHARED_CAPABILITY(...) __attribute__((acquire_shared_capability(__VA_ARGS__)))
	#define RB_RELEASE_CAPABILITY(...) __attribute__((release_capability(__VA_ARGS__)))
	#define RB_RELEASE_SHARED_CAPABILITY(...) __attribute__((release_shared_capability(__VA_ARGS__)))
	#define RB_RELEASE_GENERIC_CAPABILITY(...) __attribute__((release_generic_capability(__VA_ARGS__)))
	#define RB_TRY_ACQUIRE_CAPABILITY(...) __attribute__((try_acquire_capability(__VA_ARGS__)))
	#define RB_TRY_ACQUIRE_SHARED_CAPABILITY(...) __attribute__((try_acquire_shared_capability(__VA_ARGS__)))
	#define RB_LOCKS_EXCLUDED(...) __attribute__((locks_excluded(__VA_ARGS__)))
	#define RB_ASSERT_CAPABILITY(x) __attribute__((assert_capability(x)))
	#define RB_ASSERT_SHARED_CAPABILITY(x) __attribute__((assert_shared_capability(x)))
	#define RB_RETURN_CAPABILITY(x) __attribute__((lock_returned(x)))
	#define RB_NO_THREAD_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))
#else
	#define RB_CAPABILITY(name)
	#define RB_LOCKABLE(name)
	#define RB_SCOPED_CAPABILITY
	#define RB_SCOPED_LOCKABLE
	#define RB_GUARDED_BY(x)
	#define RB_PT_GUARDED_BY(x)
	#define RB_ACQUIRED_BEFORE(...)
	#define RB_ACQUIRED_AFTER(...)
	#define RB_REQUIRES_CAPABILITY(...)
	#define RB_REQUIRES_SHARED_CAPABILITY(...)
	#define RB_ACQUIRE_CAPABILITY(...)
	#define RB_ACQUIRE_SHARED_CAPABILITY(...)
	#define RB_RELEASE_CAPABILITY(...)
	#define RB_RELEASE_SHARED_CAPABILITY(...)
	#define RB_RELEASE_GENERIC_CAPABILITY(...)
	#define RB_TRY_ACQUIRE_CAPABILITY(...)
	#define RB_TRY_ACQUIRE_SHARED_CAPABILITY(...)
	#define RB_LOCKS_EXCLUDED(...)
	#define RB_ASSERT_CAPABILITY(x)
	#define RB_ASSERT_SHARED_CAPABILITY(x)
	#define RB_RETURN_CAPABILITY(x)
	#define RB_NO_THREAD_SAFETY_ANALYSIS
#endif

/*
 * REQUIRES: pre && post
 * ACQUIRE: !pre && post
 * RELEASE: pre && !post
 * EXCLUDES: !pre && !post
 */
