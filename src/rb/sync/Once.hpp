#pragma once

#include <rb/core/invoke.hpp>
#include <rb/core/memory/addressOf.hpp>
#include <rb/sync/impl.hpp>

namespace rb::sync {

class Once {
public:
	constexpr Once() noexcept = default;
	RB_DISABLE_COPY(Once)

	template <class Fn, class... Args>
	void operator()(Fn&& fn, Args&&... args);

private:
	struct Executor;

#if RB_USE(PTHREADS)
	pthread_once_t once_ = PTHREAD_ONCE_INIT;
#endif
};

inline thread_local void* onceCallable;
inline thread_local void (*onceCall)();

struct Once::Executor {
	template <class Fn>
	explicit Executor(Fn& fn) {
		onceCallable = core::addressOf(fn);
		onceCall = [] {
			(*static_cast<Fn*>(onceCallable))();
		};
	}

	~Executor() {
		onceCallable = nullptr;
		onceCall = nullptr;
	}
};

template <class Fn, class... Args>
void Once::operator()(Fn&& fn, Args&&... args) {
	auto invokable = [&] {
		core::invoke(RB_FWD(fn), RB_FWD(args)...);
	};

	Executor exec(invokable);
	RB_SYNC_CHECK_ERRNO(pthread_once(&once_, onceCall));
}

} // namespace rb::sync
