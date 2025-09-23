#include "OsError.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <rb/core/os.hpp>
#ifdef RB_OS_WIN
	#include <Windows.h>
#endif

using namespace rb::core;
using namespace rb::core::error;

OsError OsError::fromErrno(int error, SourceLocation const& location) {
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
    : OsError(errorCode, static_cast<RawCode>(-1), location) {
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

void OsError::printMessage(std::ostream& os) const {
	os << message(); // never null
#ifdef RB_OS_WIN
	char* buf = nullptr;
	DWORD const nChars = FormatMessageA(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	    nullptr,
	    rawCode_,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    reinterpret_cast<LPSTR>(&buf), // NOLINT(*-pro-type-reinterpret-cast)
	    0,
	    nullptr);
	if (!nChars) {
		return;
	}

	for (char* p = buf + nChars - 1;
	    p >= buf && (*p == '\n' || *p == '\r' || *p == '.');
	    --p) {
		*p = '\0';
	}
	os << " (" << buf << ")";
	LocalFree(buf);
#endif
}
