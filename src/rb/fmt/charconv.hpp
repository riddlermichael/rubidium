#pragma once

#include <rb/core/Expected.hpp>
#include <rb/core/limits.hpp>
#include <rb/core/sanitizers.hpp>
#include <rb/core/traits/IsSigned.hpp>
#include <rb/core/traits/IsUnsigned.hpp>

namespace rb::fmt {

namespace impl {
	static constexpr char kDigits[] = {
	    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	    'u', 'v', 'w', 'x', 'y', 'z'};

	template <unsigned base = 10, class T, class Char>
	constexpr void toChars(Char* ptr, T value) noexcept {
		constexpr T kBase = base;
		while (value) {
			*ptr-- = kDigits[-(value % kBase)];
			value /= kBase;
		}
	}
} // namespace impl

enum class ToCharsError {
	kOk,
	kInvalidRange,
	kValueTooLarge,
};

template <class Char>
using ToCharsResult = core::Expected<Char*, ToCharsError>;

template <unsigned base = 10, class T,
    RB_REQUIRES(2 <= base && base <= 36)>
constexpr unsigned countChars(T value) noexcept {
	if (!value) {
		return 1;
	}

	unsigned nbDigits = 0;

	if constexpr (core::isSigned<T>) {
		if (value < 0) {
			nbDigits = 1;
			if (RB_LIKELY(value != core::min<T>)) {
				value = -value;
			}
		}
	}

	constexpr T kBase = base;
	for (; value; value /= kBase) {
		++nbDigits;
	}
	return nbDigits;
}

template <class T, unsigned base = 10,
    RB_REQUIRES(2 <= base && base <= 36)>
constexpr unsigned maxChars() noexcept {
	if constexpr (core::isUnsigned<T>) {
		return countChars<base>(core::max<T>);
	} else {
		return countChars<base>(core::min<T>);
	}
}

template <unsigned base = 10, class T, class Char,
    RB_REQUIRES(core::isIntegral<T> && !core::isSame<T, bool> && 2 <= base && base <= 36)>
constexpr ToCharsResult<Char> toChars(Char* first, Char* last, T value) noexcept
// currently `detect_invalid_pointer_pairs` check is disabled on GCC only
#ifdef RB_COMPILER_CLANG
    RB_NO_SANITIZE("address")
#endif
{
	if (!first || last <= first) {
		return core::err(ToCharsError::kInvalidRange);
	}

	if (value == 0) {
		*first++ = '0';
		return first;
	}

	auto const nbChars = countChars<base>(value);
	auto* const end = first + nbChars;
	if (last < end) {
		return core::err(ToCharsError::kValueTooLarge);
	}

	if constexpr (core::isSigned<T>) {
		if (value < 0) {
			*first = '-';
			if (RB_LIKELY(value != core::min<T>)) {
				value = -value;
			}
		}
	}

	auto* ptr = end - 1;

	if constexpr (core::isSigned<T>) {
		if (RB_UNLIKELY(value == core::min<T>)) {
			impl::toChars<base>(ptr, value);
			return end;
		}
	}

	constexpr T kBase = base;
	while (value >= kBase) {
		*ptr-- = impl::kDigits[value % kBase];
		value /= kBase;
	}
	*ptr = impl::kDigits[value];
	return end;
}

enum class FromCharsError {
	kOk,
	kEmptyRange,
	kInvalidRange,
	kFormatError,
	kPosOverflow,
	kNegOverflow,
};

template <class T>
using FromCharsResult = core::Expected<T, FromCharsError>;

template <unsigned base = 10, class Char,
    RB_REQUIRES(2 <= base && base <= 36)>
constexpr core::Option<int> isDigit(Char ch) noexcept {
	auto const c = static_cast<unsigned>(ch);
	if constexpr (base <= 10) {
		return '0' <= ch && c <= '0' + base - 1
		         ? core::Option(ch - '0')
		         : core::kNone;
	} else {
		if ('0' <= ch && ch <= '9') {
			return core::Option(ch - '0');
		}
		if ('a' <= ch && c <= 'a' + base - 11) {
			return core::Option(ch - 'a' + 10);
		}
		if ('A' <= ch && c <= 'A' + base - 11) {
			return core::Option(ch - 'A' + 10);
		}
		return core::kNone;
	}
}

/// Analyzes the character sequence [@p first, @p last) for a pattern described below.
/// If no characters match the pattern or if the value obtained by parsing the matched characters
/// is not representable in the type of @p value, @p value is unmodified,
/// otherwise the characters matching the pattern are interpreted as a text representation of an arithmetic value,
/// which is stored in @p value.
/// Expects the pattern identical to the one used by std::strtol in the default ("C") locale
/// and the given non-zero numeric base, except that
/// - "0x" or "0X" prefixes are not recognized
/// - only the minus sign is recognized (not the plus sign), and only for signed integer types of @p value
/// - leading whitespace is not ignored
template <class T, unsigned base = 10, class Char,
    RB_REQUIRES(core::isIntegral<T> && !core::isSame<T, bool> && 2 <= base && base <= 36)>
[[nodiscard]] constexpr FromCharsResult<T> fromChars(Char const* first, Char const* last,
    Char const** ptr = nullptr) noexcept
#ifdef RB_COMPILER_CLANG
    RB_NO_SANITIZE("address")
#endif
{
	struct SetPtr {
		Char const** target;
		Char const*& source;

		~SetPtr() {
			if (target) {
				*target = source;
			}
		}
	};

	using core::err;

	constexpr bool kSigned = core::isSigned<T>;

	SetPtr _{ptr, first};
	if (!first || last < first) {
		return err(FromCharsError::kInvalidRange);
	}

	if (first == last) {
		return err(FromCharsError::kEmptyRange);
	}

	bool const isNeg = kSigned && *first == '-';
	if (isNeg && first + 1 == last) {
		return err(FromCharsError::kFormatError);
	}

	if (isNeg) {
		++first;
	}

	T x;
	if (auto const digit = isDigit<base>(*first); digit) {
		x = *digit;
		if (isNeg) {
			x = -x;
		}
	} else {
		return err(FromCharsError::kFormatError);
	}

	constexpr T kBase = base;
	++first;
	while (first < last) {
		auto const digit = isDigit<base>(*first);
		if (!digit) {
			break;
		}

		++first;
		T const d = *digit;
		if (x > (core::max<T> - d) / kBase) {
			return err(FromCharsError::kPosOverflow);
		}

		if constexpr (kSigned) {
			if (isNeg && x < (core::min<T> + d) / kBase) {
				return err(FromCharsError::kNegOverflow);
			}
		}

		x *= kBase;
		if constexpr (kSigned) {
			x += isNeg ? -d : d;
		} else {
			x += d;
		}
	}
	return x;
}

} // namespace rb::fmt
