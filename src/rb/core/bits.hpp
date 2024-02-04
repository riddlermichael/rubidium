#pragma once

#include <rb/core/builtins.hpp>
#include <rb/core/endian.hpp>
#include <rb/core/traits/builtins.hpp>
#include <rb/core/traits/TypeIdentity.hpp>
#include <rb/core/traits/Unsigned.hpp>

#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201'806L
	#define RB_HAS_STD_BIT_CAST 1
	#include <bit>
#elif RB_HAS_BUILTIN(__builtin_bit_cast)             \
    || defined(RB_COMPILER_MSVC) && _MSC_VER >= 1926 \
    || defined(RB_COMPILER_MINGW)
	#define RB_HAS_BUILTIN_BIT_CAST 1
#else
	#define RB_HAS_BUILTIN_BIT_CAST 0
	#include <cstring>
	#include <rb/core/traits/constructible.hpp>
#endif

#ifdef RB_COMPILER_MSVC
	#include <intrin.h>
#endif

namespace rb::core {

template <class T>
constexpr T bitMask(usize pos) noexcept {
	using U = UnsignedType<T>;
	return U{1} << pos;
}

template <class T>
constexpr void setBit(T& x, usize pos, bool value = true) noexcept {
	if (pos < 8 * sizeof(T)) {
		x = (x & ~bitMask<T>(pos)) | (value << pos);
	}
}

template <class T>
constexpr bool getBit(T x, usize pos) noexcept {
	return (pos < 8 * sizeof(T))
	    && ((x >> pos) & 1U);
}

template <class From, class To>
inline constexpr bool isBitCastable = //
    sizeof(From) == sizeof(To)
    && isTriviallyCopyable<From>
    && isTriviallyCopyable<To>
#if !RB_HAS_BUILTIN(__builtin_bit_cast) // NOLINT(*-avoid-unconditional-preprocessor-if)
    && isDefaultConstructible<To>
#endif
    ;

// Test: bitCast<i32>(3.14159265358979F) == 0x40490fdb

/// Obtain a value of type `To` by reinterpreting the object representation of `From`.
/// Every bit in the value representation of the returned `To` object is equal to the corresponding bit
/// in the object representation of @p from. The values of padding bits in the returned `To` object are unspecified.
template <class To, class From>
constexpr auto bitCast(From const& from) noexcept
    -> RB_REQUIRES_RETURN(To, isBitCastable<From, To>) {
#ifdef RB_HAS_STD_BIT_CAST
	return std::bit_cast<To>(from);
#elif RB_HAS_BUILTIN_BIT_CAST
	return __builtin_bit_cast(To, from);
#else
	static_assert(isTriviallyConstructible<To>, "Destination type must be trivially constructible");
	To to;
	std::memcpy(addressOf(to), addressOf(from), sizeof(To));
	return to;
#endif
}

template <class To>
constexpr To implicitCast(TypeIdentity<To> to) {
	return to;
}

#if defined(RB_COMPILER_GCC_LIKE)

constexpr u64 bswap(u64 value) noexcept {
	return __builtin_bswap64(value);
}

constexpr u32 bswap(u32 value) noexcept {
	return __builtin_bswap32(value);
}

constexpr u16 bswap(u16 value) noexcept {
	return __builtin_bswap16(value);
}

#else

// These definitions are written so that they are recognized by most compilers as `bswap`
// and replaced with single instruction builtins if available.

constexpr u64 bswap(u64 value) noexcept {
	return 0
	     | ((value & 0x0000'0000'0000'00ffULL) << 56)
	     | ((value & 0x0000'0000'0000'ff00ULL) << 40)
	     | ((value & 0x0000'0000'00ff'0000ULL) << 24)
	     | ((value & 0x0000'0000'ff00'0000ULL) << 8)
	     | ((value & 0x0000'00ff'0000'0000ULL) >> 8)
	     | ((value & 0x0000'ff00'0000'0000ULL) >> 24)
	     | ((value & 0x00ff'0000'0000'0000ULL) >> 40)
	     | ((value & 0xff00'0000'0000'0000ULL) >> 56);
}

constexpr u32 bswap(u32 value) noexcept {
	return 0
	     | ((value & 0x0000'00ffU) << 24)
	     | ((value & 0x0000'ff00U) << 8)
	     | ((value & 0x00ff'0000U) >> 8)
	     | ((value & 0xff00'0000U) >> 24);
}

constexpr u16 bswap(u16 value) noexcept {
	return 0
	     | ((value & 0x00ffU) << 8)
	     | ((value & 0xff00U) >> 8);
}

#endif

constexpr u8 bswap(u8 value) noexcept {
	return value;
}

// signed specializations

constexpr i64 bswap(i64 value) noexcept {
	return static_cast<i64>(bswap(static_cast<u64>(value)));
}

constexpr i32 bswap(i32 value) noexcept {
	return static_cast<i32>(bswap(static_cast<u32>(value)));
}

constexpr i16 bswap(i16 value) noexcept {
	return static_cast<i16>(bswap(static_cast<u16>(value)));
}

constexpr i8 bswap(i8 value) noexcept {
	return value;
}

// floating specializations

constexpr f32 bswap(f32 value) noexcept {
	u32 x = bitCast<u32>(value);
	x = bswap(x);
	return bitCast<f32>(x);
}

constexpr f64 bswap(f64 value) noexcept {
	u64 x = bitCast<u64>(value);
	x = bswap(x);
	return bitCast<f64>(x);
}

#if RB_IS_LITTLE_ENDIAN

template <class T>
constexpr T toLittleEndian(T value) noexcept {
	return value;
}

template <class T>
constexpr T fromLittleEndian(T value) noexcept {
	return value;
}

template <class T>
constexpr T toBigEndian(T value) noexcept {
	return bswap(value);
}

template <class T>
constexpr T fromBigEndian(T value) noexcept {
	return bswap(value);
}

#elif RB_IS_BIG_ENDIAN

template <class T>
constexpr T toLittleEndian(T value) noexcept {
	return bswap(value);
}

template <class T>
constexpr T fromLittleEndian(T value) noexcept {
	return bswap(value);
}

template <class T>
constexpr T toBigEndian(T value) noexcept {
	return value;
}

template <class T>
constexpr T fromBigEndian(T value) noexcept {
	return value;
}

#endif

} // namespace rb::core
