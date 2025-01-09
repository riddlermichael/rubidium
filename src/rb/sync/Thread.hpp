#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/time/Duration.hpp>

namespace rb::sync {

class RB_EXPORT Thread {
public:
	class RB_EXPORT Id final {
	public:
		constexpr Id() noexcept = default;

		constexpr explicit operator usize() const noexcept {
			return id_;
		}

		constexpr int opCmp(Id rhs) const noexcept;

		constexpr bool operator==(Id rhs) const noexcept;

		constexpr bool operator!=(Id rhs) const noexcept;

		constexpr bool operator<(Id rhs) const noexcept;

		constexpr bool operator<=(Id rhs) const noexcept;

		constexpr bool operator>(Id rhs) const noexcept;

		constexpr bool operator>=(Id rhs) const noexcept;

	private:
		friend class Thread;

		constexpr explicit Id(usize id) noexcept
		    : id_(id) {
		}

		usize id_ = 0;
	};

	// static Thread* currentThread() noexcept;
	static Id currentThreadId() noexcept;
	static void sleepFor(time::Duration timeout) noexcept; // TODO sleepUntil(MonoTime)
	static void yield() noexcept;

	Thread();

	virtual ~Thread() noexcept(false);

	virtual void run() = 0;

	explicit operator bool() const noexcept {
		return joinable();
	}

	// usize stackSize() const noexcept;
	// void setStackSize(usize stackSize);

	void swap(Thread& rhs) noexcept;

	Id id() const noexcept;
	[[nodiscard]] bool joinable() const noexcept;

	void detach();
	void join();
	void start(RB_SOURCE_LOCATION_DECL);

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

inline std::ostream& operator<<(std::ostream& os, Thread::Id id) {
	return os << "Thread::Id{" << static_cast<usize>(id) << "}";
}

constexpr int Thread::Id::opCmp(Id rhs) const noexcept {
	return (id_ > rhs.id_) - (id_ < rhs.id_);
}

constexpr bool Thread::Id::operator==(Id rhs) const noexcept {
	return id_ == rhs.id_;
}

constexpr bool Thread::Id::operator!=(Id rhs) const noexcept {
	return id_ != rhs.id_;
}

constexpr bool Thread::Id::operator<(Id rhs) const noexcept {
	return opCmp(rhs) < 0;
}

constexpr bool Thread::Id::operator<=(Id rhs) const noexcept {
	return opCmp(rhs) <= 0;
}

constexpr bool Thread::Id::operator>(Id rhs) const noexcept {
	return opCmp(rhs) > 0;
}

constexpr bool Thread::Id::operator>=(Id rhs) const noexcept {
	return opCmp(rhs) >= 0;
}

} // namespace rb::sync
