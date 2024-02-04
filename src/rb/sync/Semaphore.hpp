#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>

namespace rb::sync {

class RB_EXPORT Semaphore final {
public:
	explicit Semaphore(unsigned count);
	~Semaphore() noexcept(false);

	RB_DISABLE_COPY(Semaphore);

	void acquire();
	bool tryAcquire() noexcept;
	void release();

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

} // namespace rb::sync
