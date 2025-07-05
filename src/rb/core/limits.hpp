#pragma once

#include <climits>

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

#define RB_ISIZE_MIN INTPTR_MIN
#define RB_ISIZE_MAX INTPTR_MAX
#define RB_USIZE_MIN 0
#define RB_USIZE_MAX UINTPTR_MAX

#define RB_F32_MIN 0x1p-126F
#define RB_F32_MAX 0x1.fffffep127F
#define RB_F32_INF __builtin_huge_valf()
#define RB_F32_QNAN __builtin_nanf("")
#define RB_F32_SNAN __builtin_nansf("")

#define RB_F64_MIN 0x1p-1022
#define RB_F64_MAX 0x1.fffffffffffffp1023
#define RB_F64_INF __builtin_huge_val()
#define RB_F64_QNAN __builtin_nan("")
#define RB_F64_SNAN __builtin_nans("")

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
inline constexpr char max<char> = CHAR_MAX;

template <>
inline constexpr signed char max<signed char> = SCHAR_MAX;

template <>
inline constexpr short max<short> = SHRT_MAX;

template <>
inline constexpr int max<int> = INT_MAX;

template <>
inline constexpr long max<long> = LONG_MAX;

template <>
inline constexpr long long max<long long> = LLONG_MAX;

template <>
inline constexpr unsigned char max<unsigned char> = UCHAR_MAX;

template <>
inline constexpr unsigned short max<unsigned short> = USHRT_MAX;

template <>
inline constexpr unsigned max<unsigned> = UINT_MAX;

template <>
inline constexpr unsigned long max<unsigned long> = ULONG_MAX;

template <>
inline constexpr unsigned long long max<unsigned long long> = ULLONG_MAX;

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
inline constexpr char min<char> = CHAR_MIN;

template <>
inline constexpr signed char min<signed char> = SCHAR_MIN;

template <>
inline constexpr short min<short> = SHRT_MIN;

template <>
inline constexpr int min<int> = INT_MIN;

template <>
inline constexpr long min<long> = LONG_MIN;

template <>
inline constexpr long long min<long long> = LLONG_MIN;

template <>
inline constexpr unsigned char min<unsigned char> = 0;

template <>
inline constexpr unsigned short min<unsigned short> = 0;

template <>
inline constexpr unsigned min<unsigned> = 0;

template <>
inline constexpr unsigned long min<unsigned long> = 0;

template <>
inline constexpr unsigned long long min<unsigned long long> = ULLONG_MAX;

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

// quiet nan

template <class T>
struct QNaN;

template <class T>
struct QNaN<T const> : QNaN<T> {};

template <class T>
struct QNaN<T volatile> : QNaN<T> {};

template <class T>
struct QNaN<T const volatile> : QNaN<T> {};

template <class T>
inline constexpr T qnan = QNaN<T>::kValue;

template <class T>
inline constexpr T qnan<T const> = qnan<T>;

template <class T>
inline constexpr T qnan<T volatile> = qnan<T>;

template <class T>
inline constexpr T qnan<T const volatile> = qnan<T>;

template <class T>
struct QNaN {
	static constexpr T kValue = qnan<T>;
};

template <>
inline constexpr f32 qnan<f32> = RB_F32_QNAN;

template <>
inline constexpr f64 qnan<f64> = RB_F64_QNAN;

// signalling nan

template <class T>
struct SNaN;

template <class T>
struct SNaN<T const> : SNaN<T> {};

template <class T>
struct SNaN<T volatile> : SNaN<T> {};

template <class T>
struct SNaN<T const volatile> : SNaN<T> {};

template <class T>
inline constexpr T snan = SNaN<T>::kValue;

template <class T>
inline constexpr T snan<T const> = snan<T>;

template <class T>
inline constexpr T snan<T volatile> = snan<T>;

template <class T>
inline constexpr T snan<T const volatile> = snan<T>;

template <class T>
struct SNaN {
	static constexpr T kValue = snan<T>;
};

template <>
inline constexpr f32 snan<f32> = RB_F32_SNAN;

template <>
inline constexpr f64 snan<f64> = RB_F64_SNAN;

// nan

template <class T>
using NaN = QNaN<T>;

template <class T>
inline constexpr T nan = NaN<T>::kValue;

} // namespace rb::core
