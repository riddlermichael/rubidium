#include "ErrorCode.hpp"

#include <rb/core/compiler.hpp>
#include <rb/core/os.hpp>
#ifdef RB_OS_WIN
	#include <Windows.h>
#endif

namespace rb::core {

czstring toString(ErrorCode errorCode) noexcept {
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

ErrorCode fromRawError(unsigned rawCode) noexcept {
#ifdef RB_OS_WIN
	// There is no direct relationship between `GetLastError()` and `errno`,
	// so some of these choices are arbitrary (and adapted from cygwin header); see
	// https://cygwin.com/git/gitweb.cgi?p=newlib-cygwin.git;a=blob;f=winsup/cygwin/errno.cc
	switch (rawCode) {
		case ERROR_SUCCESS                   : return ErrorCode::kOk;

		case ERROR_FILE_NOT_FOUND            :
		case ERROR_PATH_NOT_FOUND            :
		case ERROR_BAD_NETPATH               :
		case ERROR_DEV_NOT_EXIST             :
		case ERROR_NETNAME_DELETED           :
		case ERROR_BAD_NET_NAME              :
		case ERROR_INVALID_NAME              :
		case ERROR_MOD_NOT_FOUND             :
		case ERROR_BAD_PATHNAME              : return ErrorCode::kNoSuchFileOrDirectory;

		case ERROR_TOO_MANY_OPEN_FILES       :
		case ERROR_NO_MORE_SEARCH_HANDLES    : return ErrorCode::kTooManyFilesOpenInSystem;

		case ERROR_ACCESS_DENIED             :
		case ERROR_NETWORK_ACCESS_DENIED     : return ErrorCode::kPermissionDenied;

		case ERROR_INVALID_HANDLE            : return ErrorCode::kBadFileDescriptor;

		case ERROR_NOT_ENOUGH_MEMORY         :
		case ERROR_OUTOFMEMORY               : return ErrorCode::kNotEnoughMemory;

		case ERROR_INVALID_DATA              :
		case ERROR_SEEK                      :
		case ERROR_SECTOR_NOT_FOUND          :
		case ERROR_INVALID_PARAMETER         :
		case ERROR_NEGATIVE_SEEK             :
		case ERROR_BAD_ARGUMENTS             :
		case ERROR_INVALID_FLAG_NUMBER       :
		case ERROR_META_EXPANSION_TOO_LONG   :
		case ERROR_INVALID_SIGNAL_NUMBER     :
		case ERROR_THREAD_1_INACTIVE         :
		case ERROR_INVALID_EA_NAME           :
		case ERROR_EA_LIST_INCONSISTENT      : return ErrorCode::kInvalidArgument;

		case ERROR_INVALID_DRIVE             :
		case ERROR_BAD_UNIT                  : return ErrorCode::kNoSuchDevice;

		case ERROR_WRITE_PROTECT             : return ErrorCode::kReadOnlyFileSystem;

		case ERROR_CRC                       :
		case ERROR_WRITE_FAULT               :
		case ERROR_READ_FAULT                :
		case ERROR_UNEXP_NET_ERR             :
		case ERROR_NET_WRITE_FAULT           :
		case ERROR_OPEN_FAILED               :
		case ERROR_LOCK_FAILED               : return ErrorCode::kIoError;

		case ERROR_SHARING_VIOLATION         :
		case ERROR_LOCK_VIOLATION            :
		case ERROR_NETWORK_BUSY              :
		case ERROR_BUSY_DRIVE                :
		case ERROR_PATH_BUSY                 :
		case ERROR_SIGNAL_PENDING            :
		case ERROR_BUSY                      :
		case ERROR_PIPE_BUSY                 : return ErrorCode::kDeviceOrResourceBusy;

		case ERROR_SHARING_BUFFER_EXCEEDED   : return ErrorCode::kNoLockAvailable;

		case ERROR_HANDLE_DISK_FULL          :
		case ERROR_DISK_FULL                 : return ErrorCode::kNoSpaceOnDevice;

		case ERROR_NOT_SUPPORTED             :
		case ERROR_BAD_NET_RESP              :
		case ERROR_CALL_NOT_IMPLEMENTED      : return ErrorCode::kFunctionNotSupported;

		case ERROR_FILE_EXISTS               :
		case ERROR_ALREADY_EXISTS            : return ErrorCode::kFileExists;

		case ERROR_CANNOT_MAKE               : return ErrorCode::kOperationNotPermitted;
		case ERROR_NO_PROC_SLOTS             : return ErrorCode::kResourceUnavailableTryAgain;

		case ERROR_BROKEN_PIPE               :
		case ERROR_BAD_PIPE                  :
		case ERROR_NO_DATA                   :
		case ERROR_PIPE_NOT_CONNECTED        : return ErrorCode::kBrokenPipe;

		case ERROR_BUFFER_OVERFLOW           : // hm okay
		case ERROR_LABEL_TOO_LONG            :
		case ERROR_FILENAME_EXCED_RANGE      : return ErrorCode::kFilenameTooLong;

		case ERROR_INSUFFICIENT_BUFFER       : return ErrorCode::kNoBufferSpace;
		case ERROR_WAIT_NO_CHILDREN          : return ErrorCode::kNoChildProcess;
		case ERROR_DIR_NOT_EMPTY             : return ErrorCode::kDirectoryNotEmpty;
		case ERROR_ATOMIC_LOCKS_NOT_SUPPORTED: return ErrorCode::kNotSupported;

		case ERROR_INVALID_EXE_SIGNATURE     :
		case ERROR_EXE_MARKED_INVALID        :
		case ERROR_BAD_EXE_FORMAT            :
		case ERROR_ITERATED_DATA_EXCEEDS_64k :
		case ERROR_INVALID_MINALLOCSIZE      :
		case ERROR_IOPL_NOT_ENABLED          :
		case ERROR_INVALID_SEGDPL            :
		case ERROR_AUTODATASEG_EXCEEDS_64k   :
		case ERROR_RELOC_CHAIN_XEEDS_SEGLIM  :
		case ERROR_INFLOOP_IN_RELOC_CHAIN    :
		case ERROR_EXE_MACHINE_TYPE_MISMATCH : return ErrorCode::kExecutableFormatError;

		case ERROR_FILE_TOO_LARGE            : return ErrorCode::kFileTooLarge;

		case ERROR_SEM_TIMEOUT               :
		case WAIT_TIMEOUT                    : return ErrorCode::kTimedOut;

		case ERROR_POSSIBLE_DEADLOCK         : return ErrorCode::kResourceDeadlockWouldOccur;

		case ERROR_SEM_OWNER_DIED            : return ErrorCode::kOwnerDead;

		// too many posts were made to semaphore;
		// in accordance with POSIX return EOVERFLOW
		case ERROR_TOO_MANY_POSTS            : return ErrorCode::kValueTooLarge;

	#ifndef RB_COMPILER_GCC_LIKE
		case ERROR_DEVICE_SUPPORT_IN_PROGRESS: return ErrorCode::kOperationInProgress;
	#endif

		default: return ErrorCode::kUnknown;
	}
#else
	return static_cast<ErrorCode>(rawCode);
#endif
}

ErrorCode fromErrno(int error) noexcept {
	return static_cast<ErrorCode>(error);
}

} // namespace rb::core
