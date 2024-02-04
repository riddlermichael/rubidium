#include "OsError.hpp"

#include <rb/core/os.hpp>
#ifdef RB_OS_WIN
	#include <Windows.h>
#endif

using namespace rb::core;
using namespace rb::core::error;

OsError OsError::fromErrno(int error, rb::core::SourceLocation const& location) {
	return OsError{::fromErrno(error), location};
}

OsError OsError::fromRawCode(RawCode rawCode, SourceLocation const& location) {
	return OsError{fromRawError(rawCode), rawCode, location};
}

OsError::RawCode OsError::lastError() noexcept {
#ifdef RB_OS_WIN
	return GetLastError();
#else
	return errno;
#endif
}

OsError OsError::lastOsError(SourceLocation const& location) noexcept {
	return fromRawCode(lastError(), location);
}

OsError::OsError(ErrorCode errorCode, SourceLocation const& location)
    : OsError(errorCode, -1, location) {
}

OsError::OsError(ErrorCode errorCode, RawCode rawCode, SourceLocation const& location)
    : Error(toString(errorCode), location)
    , errorCode_(errorCode)
    , rawCode_(rawCode) {
}

ErrorCode OsError::errorCode() const noexcept {
	return errorCode_;
}

OsError::RawCode OsError::rawCode() const noexcept {
	return rawCode_;
}
