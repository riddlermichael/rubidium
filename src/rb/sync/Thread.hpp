#pragma once

#include <rb/core/export.hpp>
#include <rb/core/memory/UniquePtr.hpp>

namespace rb::sync {

class RB_EXPORT Thread final {
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

	enum class Priority {
		kIdle,
		kLowest,
		kLow,
		kNormal,
		kHigh,
		kHighest,
		kTimeCritical,
		kInherit
	};

	using StartFn = int (*)(void*);

	static Thread* currentThread() noexcept;
	static Id currentThreadId() noexcept;
	static void yield() noexcept;

	Thread();
	~Thread() noexcept(false);

	Priority priority() const noexcept;
	void setPriority(Priority priority);

	usize stackSize() const noexcept;
	void setStackSize(usize stackSize);

	explicit operator bool() const noexcept {
		return joinable();
	}

	Id id() const noexcept;
	bool isFinished() const;
	bool isRunning() const;
	[[nodiscard]] bool joinable() const noexcept;

	void detach();
	[[noreturn]] void exit(int exitCode = 0);
	void join();
	void join(int& exitCode);

	void start(StartFn fn, void* arg, Priority priority = Priority::kInherit);

private:
	struct Impl;

	core::UniquePtr<Impl> pImpl_;
};

template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, Thread::Id id) {
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
