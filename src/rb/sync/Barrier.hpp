#pragma once

#include <rb/core/memory/UniquePtr.hpp>
#include <rb/sync.hpp>

namespace rb::sync {

class Barrier final {
public:
	explicit Barrier(unsigned count);
	~Barrier() noexcept(RB_USE(WIN32_THREADS));

	void wait() noexcept(RB_USE(WIN32_THREADS));

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

} // namespace rb::sync
