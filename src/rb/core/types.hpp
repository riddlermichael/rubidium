#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

using isize = std::intptr_t;
using usize = std::uintptr_t;

using f32 = float;
static_assert(sizeof(f32) == 4);

using f64 = double;
static_assert(sizeof(f64) == 8);

// NOLINTEND(readability-identifier-naming)

namespace rb::core {
inline namespace literals {

	constexpr i8 operator""_i8(unsigned long long value) noexcept {
		return static_cast<i8>(value);
	}

	constexpr i16 operator""_i16(unsigned long long value) noexcept {
		return static_cast<i16>(value);
	}

	constexpr i32 operator""_i32(unsigned long long value) noexcept {
		return static_cast<i32>(value);
	}

	constexpr i64 operator""_i64(unsigned long long value) noexcept {
		return static_cast<i64>(value);
	}

	constexpr u8 operator""_u8(unsigned long long value) noexcept {
		return static_cast<u8>(value);
	}

	constexpr u16 operator""_u16(unsigned long long value) noexcept {
		return static_cast<u16>(value);
	}

	constexpr u32 operator""_u32(unsigned long long value) noexcept {
		return static_cast<u32>(value);
	}

	constexpr u64 operator""_u64(unsigned long long value) noexcept {
		return static_cast<u64>(value);
	}

	// C++23's size suffixes

	constexpr isize operator""_z(unsigned long long value) noexcept {
		return static_cast<isize>(value);
	}

	constexpr isize operator""_Z(unsigned long long value) noexcept {
		return static_cast<isize>(value);
	}

	constexpr usize operator""_zu(unsigned long long value) noexcept {
		return static_cast<usize>(value);
	}

	constexpr usize operator""_uz(unsigned long long value) noexcept {
		return static_cast<usize>(value);
	}

	constexpr usize operator""_ZU(unsigned long long value) noexcept {
		return static_cast<usize>(value);
	}

	constexpr usize operator""_UZ(unsigned long long value) noexcept {
		return static_cast<usize>(value);
	}

	// C++23's floating-point literal suffixes

	constexpr f32 operator""_f32(long double value) noexcept {
		return static_cast<f32>(value);
	}

	constexpr f64 operator""_f64(long double value) noexcept {
		return static_cast<f64>(value);
	}

} // namespace literals
} // namespace rb::core
