#pragma once

#include <rb/core/error/Error.hpp>
#include <rb/core/helpers.hpp>
#include <rb/core/move.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {
inline namespace error {

	class RangeError final : public Error {
		static constexpr LiteralString kDefaultMessage = "Out of range";

	public:
		// TODO
		template <class T>
		static LiteralString makeMessage(T const& value, TypeIdentity<T> const& begin, TypeIdentity<T> const& end,
		    bool excludeEnd = true) {
			RB_UNUSED(value);
			RB_UNUSED(begin);
			RB_UNUSED(end);
			RB_UNUSED(excludeEnd);
			return kDefaultMessage;
		}

		explicit RangeError(LiteralString message = kDefaultMessage, RB_SOURCE_LOCATION_DECL) noexcept
		    : Error(message, location) {
		}

		template <class T>
		RangeError(T const& value, TypeIdentity<T> const& begin, TypeIdentity<T> const& end,
		    bool excludeEnd = true, RB_SOURCE_LOCATION_DECL)
		    : Error(makeMessage(value, begin, end, excludeEnd), location) {
		}
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

	/// Check if @p value is in range [@p begin, @p end).
	/// @throw RangeError @p value is ot of range [@p begin, @p end).
	template <bool excludeEnd = true, class T>
	constexpr void checkRange(T const& value, TypeIdentity<T> const& begin, TypeIdentity<T> const& end,
	    RB_SOURCE_LOCATION_DECL) {
		if (value < begin || (excludeEnd ? !(value < end) : (end < value))) {
			throw RangeError(value, begin, end, excludeEnd, location);
		}
	}

#pragma clang diagnostic pop

} // namespace error
} // namespace rb::core

#define RB_CHECK_RANGE(value, begin, end) rb::core::checkRange((value), (begin), (end), RB_SOURCE_LOCATION)
#define RB_CHECK_RANGE_EXCL(value, begin, end) rb::core::checkRange<false>((value), (begin), (end), RB_SOURCE_LOCATION)

#define RB_RANGE_ASSERT_MSG(cond, msg)                           \
	(cond) ? static_cast<void>(0) : throw rb::core::RangeError { \
		msg, RB_SOURCE_LOCATION                                  \
	}

#define RB_RANGE_ASSERT(cond) RB_RANGE_ASSERT_MSG((cond), #cond)

// TODO add RB_CHECK_{LT,LE,GE,GT}
