#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <rb/core/attributes.hpp>
#include <rb/core/compiler.hpp>
#include <rb/core/types.hpp>

#ifdef RB_COMPILER_MSVC
	#include <rb/core/bits.hpp>
#endif

namespace rb::math {

enum class RB_CLOSED_ENUM FloatingPointCategory {
	kInfinity,
	kNaN,
	kNormal,
	kSubnormal,
	kZero,
};

#ifdef RB_COMPILER_MSVC

constexpr FloatingPointCategory classify(f32 value) noexcept {
	constexpr u32 exponentMask = 0x7F800000U;
	constexpr u32 mantissaMask = 0x007FFFFFU;

	u32 const bits = core::bitCast<u32>(value);
	u32 const exponent = bits & exponentMask;
	u32 const mantissa = bits & mantissaMask;
	switch (exponent) {
		case 0: return mantissa
			? FloatingPointCategory::kSubnormal
			: FloatingPointCategory::kZero;

		case exponentMask: return mantissa
			? FloatingPointCategory::kNaN
			: FloatingPointCategory::kInfinity;

		default: return FloatingPointCategory::kNormal;
	}
}

constexpr FloatingPointCategory classify(f64 value) noexcept {
	constexpr u64 exponentMask = 0x7FF0000000000000ULL;
	constexpr u64 mantissaMask = 0x000FFFFFFFFFFFFFULL;

	u64 const bits = core::bitCast<u64>(value);
	u64 const exponent = bits & exponentMask;
	u64 const mantissa = bits & mantissaMask;
	switch (exponent) {
		case 0: return mantissa
			? FloatingPointCategory::kSubnormal
			: FloatingPointCategory::kZero;

		case exponentMask: return mantissa
			? FloatingPointCategory::kNaN
			: FloatingPointCategory::kInfinity;

		default: return FloatingPointCategory::kNormal;
	}
}

constexpr bool isNaN(f32 value) noexcept {
	return classify(value) == FloatingPointCategory::kNaN;
}

constexpr bool isNaN(f64 value) noexcept {
	return classify(value) == FloatingPointCategory::kNaN;
}

constexpr bool isNegative(f32 value) noexcept {
	u32 const bits = core::bitCast<u32>(value);
	return bits >> 31;
}

constexpr bool isNegative(f64 value) noexcept {
	u64 const bits = core::bitCast<u64>(value);
	return bits >> 63;
}

#else

constexpr FloatingPointCategory classify(f32 value) noexcept {
	return static_cast<FloatingPointCategory>(__builtin_fpclassify(
	    static_cast<int>(FloatingPointCategory::kNaN),
	    static_cast<int>(FloatingPointCategory::kInfinity),
	    static_cast<int>(FloatingPointCategory::kNormal),
	    static_cast<int>(FloatingPointCategory::kSubnormal),
	    static_cast<int>(FloatingPointCategory::kZero),
	    value));
}

constexpr FloatingPointCategory classify(f64 value) noexcept {
	return static_cast<FloatingPointCategory>(__builtin_fpclassify(
	    static_cast<int>(FloatingPointCategory::kNaN),
	    static_cast<int>(FloatingPointCategory::kInfinity),
	    static_cast<int>(FloatingPointCategory::kNormal),
	    static_cast<int>(FloatingPointCategory::kSubnormal),
	    static_cast<int>(FloatingPointCategory::kZero),
	    value));
}

constexpr bool isNaN(f32 value) noexcept {
	return __builtin_isnan(value);
}

constexpr bool isNaN(f64 value) noexcept {
	return __builtin_isnan(value);
}

constexpr bool isNegative(f32 value) noexcept {
	return __builtin_signbit(value);
}

constexpr bool isNegative(f64 value) noexcept {
	return __builtin_signbit(value);
}

#endif

} // namespace rb::math
