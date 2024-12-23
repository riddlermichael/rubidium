#pragma once

#include <rb/core/endian.hpp>
#include <rb/core/types.hpp>

namespace rb::time::impl {

class I64 final {
public:
	constexpr I64() noexcept = default;

	constexpr explicit I64(i64 value) noexcept {
		*this = value;
	}

	constexpr I64& operator=(i64 value) noexcept {
		hi_ = value >> 32;
		lo_ = value;
		return *this;
	}

	constexpr explicit operator bool() const noexcept {
		return hi_ || lo_;
	}

	constexpr I64& operator++() noexcept {
		return *this = value() + 1;
	}

	constexpr I64& operator--() noexcept {
		return *this = value() - 1;
	}

	constexpr I64& operator+=(I64 rhs) noexcept {
		return *this = value() + rhs.value();
	}

	constexpr I64& operator-=(I64 rhs) noexcept {
		return *this = value() - rhs.value();
	}

	// ReSharper disable once CppNonExplicitConversionOperator
	constexpr operator i64() const noexcept { // NOLINT(*-explicit-constructor)
		return value();
	}

	constexpr i64 value() const noexcept {
		return static_cast<i64>(static_cast<u64>(hi_) << 32 | lo_);
	}

private:
#if RB_IS_BIG_ENDIAN
	u32 hi_ = 0;
	u32 lo_ = 0;
#else
	u32 lo_ = 0;
	u32 hi_ = 0;
#endif
};

} // namespace rb::time::impl
