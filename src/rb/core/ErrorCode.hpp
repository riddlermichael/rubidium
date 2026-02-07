#pragma once

#include <cerrno>

#include <rb/core/attributes.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

/// Twin of [`std::errc`](https://en.cppreference.com/w/cpp/error/errc).
/// @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/errno-constants?view=msvc-170
enum class RB_OPEN_ENUM ErrorCode {
	kNotImplemented = -2,
	kUnknown = -1,
	kOk = 0,
	kAddressFamilyNotSupported = EAFNOSUPPORT,
	kAddressInUse = EADDRINUSE,
	kAddressNotAvailable = EADDRNOTAVAIL,
	kAlreadyConnected = EISCONN,
	kArgumentListTooLong = E2BIG,
	kArgumentOutOfDomain = EDOM,
	kBadAddress = EFAULT,
	kBadFileDescriptor = EBADF,
	kBadMessage = EBADMSG,
	kBrokenPipe = EPIPE,
	kConnectionAborted = ECONNABORTED,
	kConnectionAlreadyInProgress = EALREADY,
	kConnectionRefused = ECONNREFUSED,
	kConnectionReset = ECONNRESET,
	kCrossDeviceLink = EXDEV,
	kDestinationAddressRequired = EDESTADDRREQ,
	kDeviceOrResourceBusy = EBUSY,
	kDirectoryNotEmpty = ENOTEMPTY,
	kExecutableFormatError = ENOEXEC,
	kFileExists = EEXIST,
	kFileTooLarge = EFBIG,
	kFilenameTooLong = ENAMETOOLONG,
	kFunctionNotSupported = ENOSYS,
	kHostUnreachable = EHOSTUNREACH,
	kIdentifierRemoved = EIDRM,
	kIllegalByteSequence = EILSEQ,
	kInappropriateIoControlOperation = ENOTTY,
	kInterrupted = EINTR,
	kInvalidArgument = EINVAL,
	kInvalidSeek = ESPIPE,
	kIoError = EIO,
	kIsADirectory = EISDIR,
	kMessageSize = EMSGSIZE,
	kNetworkDown = ENETDOWN,
	kNetworkReset = ENETRESET,
	kNetworkUnreachable = ENETUNREACH,
	kNoBufferSpace = ENOBUFS,
	kNoChildProcess = ECHILD,
	kNoLink = ENOLINK,
	kNoLockAvailable = ENOLCK,
	kNoMessage = ENOMSG,
	kNoProtocolOption = ENOPROTOOPT,
	kNoSpaceOnDevice = ENOSPC,
	kNoSuchDeviceOrAddress = ENXIO,
	kNoSuchDevice = ENODEV,
	kNoSuchFileOrDirectory = ENOENT,
	kNoSuchProcess = ESRCH,
	kNotADirectory = ENOTDIR,
	kNotASocket = ENOTSOCK,
	kNotConnected = ENOTCONN,
	kNotEnoughMemory = ENOMEM,
	kNotSupported = ENOTSUP,
	kOperationCanceled = ECANCELED,
	kOperationInProgress = EINPROGRESS,
	kOperationNotPermitted = EPERM,
	kOperationNotSupported = EOPNOTSUPP,
	kOperationWouldBlock = EWOULDBLOCK,
	kOwnerDead = EOWNERDEAD,
	kPermissionDenied = EACCES,
	kProtocolError = EPROTO,
	kProtocolNotSupported = EPROTONOSUPPORT,
	kReadOnlyFileSystem = EROFS,
	kResourceDeadlockWouldOccur = EDEADLK,
	kResourceUnavailableTryAgain = EAGAIN,
	kResultOutOfRange = ERANGE,
	kStateNotRecoverable = ENOTRECOVERABLE,
	kTextFileBusy = ETXTBSY,
	kTimedOut = ETIMEDOUT,
	kTooManyFilesOpenInSystem = ENFILE,
	kTooManyFilesOpen = EMFILE,
	kTooManyLinks = EMLINK,
	kTooManySymbolicLinkLevels = ELOOP,
	kValueTooLarge = EOVERFLOW,
	kWrongProtocolType = EPROTOTYPE,
};

/**
 * Returns textual description of the @p errorCode.
 * The contents of the string are *locale-independent*.
 * @return pointer to a null-terminated string corresponding to the @p errorCode
 */
czstring toString(ErrorCode errorCode) noexcept;

/// @return error code converted from OS' ("raw") error @p rawCode.
ErrorCode fromRawError(unsigned rawCode) noexcept;

/// @return error code converted from error number @p error.
ErrorCode fromErrno(int error = errno) noexcept;

} // namespace rb::core
