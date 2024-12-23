#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>
#include <rb/time/Time.hpp>

namespace rb::sync {

class RB_EXPORT OsThread final {
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
		friend class OsThread;

		constexpr explicit Id(usize id) noexcept
		    : id_(id) {
		}

		usize id_ = 0;
	};

	using StartFn = int (*)(void*);

	// static Thread* currentThread() noexcept;
	// static Id currentThreadId() noexcept;
	// static void sleepFor(time::Duration timeout) noexcept;
	// static void sleepUntil(time::Time deadline) noexcept;
	// static void yield() noexcept;

	OsThread();
	~OsThread() noexcept(false);

	// explicit operator bool() const noexcept {
	// 	return joinable();
	// }

	// usize stackSize() const noexcept;
	// void setStackSize(usize stackSize);

	// Id id() const noexcept;
	// [[nodiscard]] bool joinable() const noexcept;

	// void detach();
	// [[noreturn]] void exit(int exitCode = 0);
	// void join();
	// void join(int& exitCode);
	//void swap(Thread& rhs) noexcept;

	void start(StartFn fn, void* arg);

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

inline std::ostream& operator<<(std::ostream& os, OsThread::Id id) {
	return os << "Thread::Id{" << static_cast<usize>(id) << "}";
}

constexpr int OsThread::Id::opCmp(Id rhs) const noexcept {
	return (id_ > rhs.id_) - (id_ < rhs.id_);
}

constexpr bool OsThread::Id::operator==(Id rhs) const noexcept {
	return id_ == rhs.id_;
}

constexpr bool OsThread::Id::operator!=(Id rhs) const noexcept {
	return id_ != rhs.id_;
}

constexpr bool OsThread::Id::operator<(Id rhs) const noexcept {
	return opCmp(rhs) < 0;
}

constexpr bool OsThread::Id::operator<=(Id rhs) const noexcept {
	return opCmp(rhs) <= 0;
}

constexpr bool OsThread::Id::operator>(Id rhs) const noexcept {
	return opCmp(rhs) > 0;
}

constexpr bool OsThread::Id::operator>=(Id rhs) const noexcept {
	return opCmp(rhs) >= 0;
}

} // namespace rb::sync
