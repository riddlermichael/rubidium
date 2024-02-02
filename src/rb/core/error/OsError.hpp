#pragma once

#include <rb/core/error/Error.hpp>
#include <rb/core/ErrorCode.hpp>

namespace rb::core {
inline namespace error {

	class RB_EXPORT OsError : public Error {
	public:
		using RawCode = unsigned;

		static OsError fromRawCode(RawCode rawCode, RB_SOURCE_LOCATION_DECL);
		static RawCode lastError() noexcept;
		static OsError lastOsError(RB_SOURCE_LOCATION_DECL) noexcept;

		explicit OsError(ErrorCode errorCode, RawCode rawCode = -1, RB_SOURCE_LOCATION_DECL);

		ErrorCode errorCode() const noexcept;
		RawCode rawCode() const noexcept;

	private:
		ErrorCode errorCode_;
		RawCode rawCode_;
	};

} // namespace error
} // namespace rb::core
