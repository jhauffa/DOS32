
#include <cerrno>
#include <cstring>

#include "os/unix/UnixException.h"


namespace host {

UnixException::UnixException()
{
	mError = errno;
}

UnixException::UnixException( int error ) : mError( error )
{
}

std::string UnixException::getErrorMessage() const
{
	char buf[256];
	strerror_r( mError, buf, sizeof( buf ) );
	return std::string( buf );
}

int UnixException::getError() const
{
	switch ( mError )
	{
	case EPERM:
	case EACCES:
		return OS_ACCESS_DENIED;
	case ENOENT:
	case EISDIR:
		return OS_FILE_NOT_FOUND;
	case EIO:
	case ENOSPC:
		return OS_IO_ERROR;
	case EBADF:
	case EINVAL:
		return OS_INVALID_PARAMETER;
	case ENOMEM:
	case ENFILE:
	case EMFILE:
		return OS_OUT_OF_MEMORY;
	case EFAULT:
		return OS_BAD_ADDRESS;
	default:
		return OS_UNKNOWN_ERROR;
	}
}

}
