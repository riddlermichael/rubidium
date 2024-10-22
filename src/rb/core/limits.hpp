#pragma once

#include <rb/core/processor.hpp>
#include <rb/core/types.hpp>

// NOLINTBEGIN(*-macro-to-enum)

#define RB_I8_MIN (-127 - 1)
#define RB_I8_MAX 127
#define RB_I16_MIN (-32767 - 1)
#define RB_I16_MAX 32767
#define RB_I32_MIN (-2147483647 - 1)
#define RB_I32_MAX 2147483647
#define RB_I64_MAX 9223372036854775807LL
#define RB_I64_MIN (-RB_I64_MAX - 1LL)

#define RB_U8_MIN 0U
#define RB_U8_MAX 255U
#define RB_U16_MIN 0U
#define RB_U16_MAX 65535U
#define RB_U32_MIN 0U
#define RB_U32_MAX 4294967295U
#define RB_U64_MIN 0ULL
#define RB_U64_MAX 18446744073709551615ULL

#if RB_IS_64BIT
	#define RB_ISIZE_MIN RB_I64_MIN
	#define RB_ISIZE_MAX RB_I64_MAX
	#define RB_USIZE_MIN RB_U64_MIN
	#define RB_USIZE_MAX RB_U64_MAX
#elif RB_IS_32BIT
	#define RB_ISIZE_MIN RB_I32_MIN
	#define RB_ISIZE_MAX RB_I32_MAX
	#define RB_USIZE_MIN RB_U32_MIN
	#define RB_USIZE_MAX RB_U32_MAX
#endif

#define RB_F32_MIN 0x1p-126F
#define RB_F32_MAX 0x1.fffffep127F
#define RB_F32_INF __builtin_huge_valf()
#define RB_F64_MIN 0x1p-1022
#define RB_F64_MAX 0x1.fffffffffffffp1023
#define RB_F64_INF __builtin_huge_val()

// NOLINTEND(*-macro-to-enum)

namespace rb::core {

// max

template <class T>
struct Max;

template <class T>
struct Max<T const> : Max<T> {};

template <class T>
struct Max<T volatile> : Max<T> {};

template <class T>
struct Max<T const volatile> : Max<T> {};

template <class T>
inline constexpr T max = Max<T>::kValue;

template <class T>
inline constexpr T max<T const> = max<T>;

template <class T>
inline constexpr T max<T volatile> = max<T>;

template <class T>
inline constexpr T max<T const volatile> = max<T>;

template <class T>
struct Max {
	static constexpr T kValue = max<T>;
};

template <>
inline constexpr i8 max<i8> = RB_I8_MAX;

template <>
inline constexpr i16 max<i16> = RB_I16_MAX;

template <>
inline constexpr i32 max<i32> = RB_I32_MAX;

template <>
inline constexpr i64 max<i64> = RB_I64_MAX;

template <>
inline constexpr u8 max<u8> = RB_U8_MAX;

template <>
inline constexpr u16 max<u16> = RB_U16_MAX;

template <>
inline constexpr u32 max<u32> = RB_U32_MAX;

template <>
inline constexpr u64 max<u64> = RB_U64_MAX;

template <>
inline constexpr f32 max<f32> = RB_F32_MAX;

template <>
inline constexpr f64 max<f64> = RB_F64_MAX;

// min

template <class T>
struct Min;

template <class T>
struct Min<T const> : Min<T> {};

template <class T>
struct Min<T volatile> : Min<T> {};

template <class T>
struct Min<T const volatile> : Min<T> {};

template <class T>
inline constexpr T min = Min<T>::kValue;

template <class T>
inline constexpr T min<T const> = min<T>;

template <class T>
inline constexpr T min<T volatile> = min<T>;

template <class T>
inline constexpr T min<T const volatile> = min<T>;

template <class T>
struct Min {
	static constexpr T kValue = min<T>;
};

template <>
inline constexpr i8 min<i8> = RB_I8_MIN;

template <>
inline constexpr i16 min<i16> = RB_I16_MIN;

template <>
inline constexpr i32 min<i32> = RB_I32_MIN;

template <>
inline constexpr i64 min<i64> = RB_I64_MIN;

template <>
inline constexpr u8 min<u8> = RB_U8_MIN;

template <>
inline constexpr u16 min<u16> = RB_U16_MIN;

template <>
inline constexpr u32 min<u32> = RB_U32_MIN;

template <>
inline constexpr u64 min<u64> = RB_U64_MIN;

template <>
inline constexpr f32 min<f32> = RB_F32_MIN;

template <>
inline constexpr f64 min<f64> = RB_F64_MIN;

// inf

template <class T>
struct Inf;

template <class T>
struct Inf<T const> : Inf<T> {};

template <class T>
struct Inf<T volatile> : Inf<T> {};

template <class T>
struct Inf<T const volatile> : Inf<T> {};

template <class T>
inline constexpr T inf = Inf<T>::kValue;

template <class T>
inline constexpr T inf<T const> = inf<T>;

template <class T>
inline constexpr T inf<T volatile> = inf<T>;

template <class T>
inline constexpr T inf<T const volatile> = inf<T>;

template <class T>
struct Inf {
	static constexpr T kValue = inf<T>;
};

template <>
inline constexpr f32 inf<f32> = RB_F32_INF;

template <>
inline constexpr f64 inf<f64> = RB_F64_INF;

} // namespace rb::core
