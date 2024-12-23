#include "Thread.hpp"

#include <memory>
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
#endif

Thread::Thread()
    : pImpl_(core::makeUnique<Impl>()) {
}

Thread::~Thread() noexcept(false) {
	// if (joinable()) {
	// 	throw OsError(ErrorCode::kOperationInProgress);
	// }
}

// bool Thread::joinable() const noexcept {
// 	return false;
// }
