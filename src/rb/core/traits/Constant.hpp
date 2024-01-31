#pragma once

#include <rb/core/traits/remove.hpp>

// NOLINTBEGIN(readability-identifier-naming)

namespace rb::core {

namespace impl {

	template <auto v, bool isIntegral = false>
	struct ConstantHolder {
		static constexpr auto value = v;
	};

	template <auto v>
	struct ConstantHolder<v, true> {
		enum : decltype(v) {
			value = v
		};
	};

	template <class T, class U>
	struct IsSameImpl {
		enum : bool {
			kValue = false
		};
	};

	template <class T>
	struct IsSameImpl<T, T> {
		enum : bool {
			kValue = true
		};
	};

	template <class T, class... Ts>
	inline constexpr bool isAnyOf = (... || IsSameImpl<T, Ts>::kValue);

	template <class T>
	inline constexpr bool isIntegralImpl = isAnyOf<RemoveCv<T>,
	    bool,
	    char,
	    signed char,
	    unsigned char,
#ifdef __cpp_char8_t
	    char8_t,
#endif
	    wchar_t,
	    char16_t,
	    char32_t,
	    short,
	    unsigned short,
	    int,
	    unsigned,
	    long,
	    unsigned long,
	    long long,
	    unsigned long long>;

} // namespace impl

inline namespace traits {

	template <auto v>
	struct Constant : impl::ConstantHolder<v, impl::isIntegralImpl<decltype(v)>> {
		using value_type = decltype(v);
		using type = Constant;

		constexpr operator value_type() const noexcept { // NOLINT(google-explicit-constructor)
			return Constant::value;
		}

		constexpr value_type operator()() const noexcept {
			return Constant::value;
		}
	};

} // namespace traits

} // namespace rb::core

// NOLINTEND(readability-identifier-naming)
