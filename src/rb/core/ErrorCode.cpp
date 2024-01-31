#include "ErrorCode.hpp"

char const* rb::core::toString(ErrorCode errorCode) noexcept {
	// EOPNOTSUPP and ENOTSUP may be identical
	if (errorCode == ErrorCode::kNotSupported) {
		return "Operation not supported";
	}

	// EAGAIN and EWOULDBLOCK may be identical
	if (errorCode == ErrorCode::kResourceUnavailableTryAgain) {
		return "Resource unavailable, try again";
	}

	switch (errorCode) {
		case ErrorCode::kNotImplemented                 : return "Not implemented";
		case ErrorCode::kUnknown                        : return "Unknown error";
		case ErrorCode::kOk                             : return "Ok";
		case ErrorCode::kAddressFamilyNotSupported      : return "Address family not supported";
		case ErrorCode::kAddressInUse                   : return "Address in use";
		case ErrorCode::kAddressNotAvailable            : return "Address not available";
		case ErrorCode::kAlreadyConnected               : return "Socket is connected";
		case ErrorCode::kArgumentListTooLong            : return "Argument list too long";
		case ErrorCode::kArgumentOutOfDomain            : return "Mathematics argument out of domain of function";
		case ErrorCode::kBadAddress                     : return "Bad address";
		case ErrorCode::kBadFileDescriptor              : return "Bad file descriptor";
		case ErrorCode::kBadMessage                     : return "Bad message";
		case ErrorCode::kBrokenPipe                     : return "Broken pipe";
		case ErrorCode::kConnectionAborted              : return "Connection aborted";
		case ErrorCode::kConnectionAlreadyInProgress    : return "Connection already in progress";
		case ErrorCode::kConnectionRefused              : return "Connection refused";
		case ErrorCode::kConnectionReset                : return "Connection reset";
		case ErrorCode::kCrossDeviceLink                : return "Improper link";
		case ErrorCode::kDestinationAddressRequired     : return "Destination address required";
		case ErrorCode::kDeviceOrResourceBusy           : return "Device or resource busy";
		case ErrorCode::kDirectoryNotEmpty              : return "Directory not empty";
		case ErrorCode::kExecutableFormatError          : return "Executable file format error";
		case ErrorCode::kFileExists                     : return "File exists";
		case ErrorCode::kFileTooLarge                   : return "File too large";
		case ErrorCode::kFilenameTooLong                : return "Filename too long";
		case ErrorCode::kFunctionNotSupported           : return "Function not supported";
		case ErrorCode::kHostUnreachable                : return "Host is unreachable";
		case ErrorCode::kIdentifierRemoved              : return "Identifier removed";
		case ErrorCode::kIllegalByteSequence            : return "Illegal byte sequence";
		case ErrorCode::kInappropriateIoControlOperation: return "Inappropriate I/O control operation";
		case ErrorCode::kInterrupted                    : return "Interrupted function call";
		case ErrorCode::kInvalidArgument                : return "Invalid argument";
		case ErrorCode::kInvalidSeek                    : return "Invalid seek";
		case ErrorCode::kIoError                        : return "Input/output error";
		case ErrorCode::kIsADirectory                   : return "Is a directory";
		case ErrorCode::kMessageSize                    : return "Message too long";
		case ErrorCode::kNetworkDown                    : return "Network is down";
		case ErrorCode::kNetworkReset                   : return "Connection aborted by network";
		case ErrorCode::kNetworkUnreachable             : return "Network unreachable";
		case ErrorCode::kNoBufferSpace                  : return "No buffer space available";
		case ErrorCode::kNoChildProcess                 : return "No child processes";
		case ErrorCode::kNoLink                         : return "Link has been severed";
		case ErrorCode::kNoLockAvailable                : return "No locks available";
		case ErrorCode::kNoMessage                      : return "No message of the desired type";
		case ErrorCode::kNoProtocolOption               : return "Protocol not available";
		case ErrorCode::kNoSpaceOnDevice                : return "No space left on device";
		case ErrorCode::kNoSuchDeviceOrAddress          : return "No such device or address";
		case ErrorCode::kNoSuchDevice                   : return "No such device";
		case ErrorCode::kNoSuchFileOrDirectory          : return "No such file or directory";
		case ErrorCode::kNoSuchProcess                  : return "No such process";
		case ErrorCode::kNotADirectory                  : return "Not a directory";
		case ErrorCode::kNotASocket                     : return "Not a socket";
		case ErrorCode::kNotConnected                   : return "The socket is not connected";
		case ErrorCode::kNotEnoughMemory                : return "Not enough space";
		case ErrorCode::kOperationCanceled              : return "Operation canceled";
		case ErrorCode::kOperationInProgress            : return "Operation in progress";
		case ErrorCode::kOperationNotPermitted          : return "Operation not permitted";
		case ErrorCode::kOperationNotSupported          : return "Operation not supported on socket";
		case ErrorCode::kOperationWouldBlock            : return "Operation would block";
		case ErrorCode::kOwnerDead                      : return "Owner died";
		case ErrorCode::kPermissionDenied               : return "Permission denied";
		case ErrorCode::kProtocolError                  : return "Protocol error";
		case ErrorCode::kProtocolNotSupported           : return "Protocol not supported";
		case ErrorCode::kReadOnlyFileSystem             : return "Read-only filesystem";
		case ErrorCode::kResourceDeadlockWouldOccur     : return "Resource deadlock would occur";
		case ErrorCode::kResultOutOfRange               : return "Result too large";
		case ErrorCode::kStateNotRecoverable            : return "State not recoverable";
		case ErrorCode::kTextFileBusy                   : return "Text file busy";
		case ErrorCode::kTimedOut                       : return "Connection timed out";
		// on Linux, this is probably a result of encountering the /proc/sys/fs/file-max limit (see proc(5))
		case ErrorCode::kTooManyFilesOpenInSystem       : return "Too many open files in system";
		/* Commonly caused by exceeding the RLIMIT_NOFILE resource limit described in getrlimit(2).
		 * Can also be caused by exceeding the limit specified in /proc/sys/fs/nr_open. */
		// File descriptor value too large (in C++)
		case ErrorCode::kTooManyFilesOpen               : return "Too many open files";
		case ErrorCode::kTooManyLinks                   : return "Too many links";
		case ErrorCode::kTooManySymbolicLinkLevels      : return "Too many levels of symbolic links";
		case ErrorCode::kValueTooLarge                  : return "Value too large to be stored in data type";
		case ErrorCode::kWrongProtocolType              : return "Protocol wrong type for socket";

		default                                         : return "Undefined error";
	}
}
