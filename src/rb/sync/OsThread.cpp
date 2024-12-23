#include "OsThread.hpp"

#include <memory>
#include <rb/sync/impl.hpp>

using namespace rb::core;
using namespace rb::sync;

#if RB_USE(PTHREADS)

namespace {

struct ThreadData {
	OsThread::StartFn fn;
	void* arg;
};

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

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void* threadFunc(void* arg) {
	auto const* data = static_cast<ThreadData const*>(arg);
	isize const ret = (data->fn)(data->arg);
	return reinterpret_cast<void*>(ret); // NOLINT(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
}

} // namespace

struct OsThread::Impl {
	pthread_t impl = {};
	UniquePtr<ThreadData> data;
};

void OsThread::start(StartFn fn, void* arg) {
	pthread_attr_t attr{};
	AttributeGuard const _{attr};
	RB_SYNC_CHECK_ERRNO(pthread_attr_init(&attr));
	RB_SYNC_CHECK_ERRNO(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	pImpl_->data = makeUnique<ThreadData>();
	pImpl_->data->fn = fn;
	pImpl_->data->arg = arg;
	RB_SYNC_CHECK_ERRNO(pthread_create(RB_SYNC_IMPL, &attr, threadFunc, pImpl_->data.get()));
	RB_SYNC_CHECK_ERRNO(pthread_attr_destroy(&attr));
}

#elif RB_USE(WIN32_THREADS)
#endif

OsThread::OsThread()
    : pImpl_(core::makeUnique<Impl>()) {
}

OsThread::~OsThread() noexcept(false) {
	// if (joinable()) {
	// 	throw OsError(ErrorCode::kOperationInProgress);
	// }
}

// bool Thread::joinable() const noexcept {
// 	return false;
// }
