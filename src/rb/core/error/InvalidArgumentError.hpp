#pragma once

#include <rb/core/error/Error.hpp>

namespace rb::core {
inline namespace error {

	class InvalidArgumentError final : public Error {
	public:
		using Error::Error;
	};

} // namespace error
} // namespace rb::core
