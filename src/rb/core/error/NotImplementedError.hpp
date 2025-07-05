#pragma once

#include <rb/core/error/Error.hpp>

namespace rb::core {
inline namespace error {

	class RB_EXPORT NotImplementedError final : public Error {
	public:
		explicit NotImplementedError(RB_SOURCE_LOCATION_DECL) noexcept
		    : Error("Not implemented yet", location) {
		}
	};

} // namespace error
} // namespace rb::core

#define RB_NOT_IMPLEMENTED() throw rb::core::NotImplementedError(RB_SOURCE_LOCATION)
