#pragma once

#include <rb/core/error/Error.hpp>
#include <rb/core/ErrorCode.hpp>

namespace rb::core {
inline namespace error {

	// ReSharper disable once CppClassCanBeFinal
	class RB_EXPORT OsError : public Error {
	public:
		using RawCode = unsigned;

		static OsError fromErrno(int error = errno, RB_SOURCE_LOCATION_DECL);
		static OsError fromRawCode(RawCode rawCode, RB_SOURCE_LOCATION_DECL);
		static RawCode lastError() noexcept;
		static OsError lastOsError(RB_SOURCE_LOCATION_DECL) noexcept;

		explicit OsError(ErrorCode errorCode, RB_SOURCE_LOCATION_DECL);
		OsError(ErrorCode errorCode, RawCode rawCode, RB_SOURCE_LOCATION_DECL);

		ErrorCode errorCode() const noexcept;
		RawCode rawCode() const noexcept;

	protected:
		void printMessage(std::ostream& os) const override;

	private:
		ErrorCode errorCode_;
		RawCode rawCode_;
	};

} // namespace error
} // namespace rb::core
