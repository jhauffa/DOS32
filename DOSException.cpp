
#include "os/OSException.h"
#include "DOSException.h"


DOSException::DOSException( uint8_t errorCode ) : mErrorCode ( errorCode ),
	mErrorClass( CLASS_UNKNOWN ), mRecommendedAction( ACTION_CLEANUP_ABORT ),
	mErrorLocus( LOCUS_UNKNOWN )
{
}

DOSException::DOSException( const host::OSException &ex ) : mErrorClass( CLASS_UNKNOWN ),
	mRecommendedAction( ACTION_CLEANUP_ABORT ), mErrorLocus( LOCUS_UNKNOWN )
{
	// TODO: move into helper function, take "error locus" as parameter
	switch ( ex.getError() )
	{
	case host::OSException::OS_NO_ERROR:
		mErrorCode = ERROR_NO_ERROR;
		break;
	case host::OSException::OS_ACCESS_DENIED:
		mErrorCode = ERROR_ACCESS_DENIED;
		break;
	case host::OSException::OS_INVALID_PARAMETER:
		mErrorCode = ERROR_INVALID_DATA;
		break;
	case host::OSException::OS_IO_ERROR:
		mErrorCode = ERROR_GENERAL_FAILURE;	// TODO: report read or write error
		break;
	case host::OSException::OS_FILE_NOT_FOUND:
		mErrorCode = ERROR_FILE_NOT_FOUND;
		break;
	case host::OSException::OS_OUT_OF_MEMORY:
		mErrorCode = ERROR_OUT_OF_MEMORY;
		break;
	case host::OSException::OS_BAD_ADDRESS:
		mErrorCode = ERROR_GENERAL_FAILURE;
		break;
	case host::OSException::OS_UNKNOWN_ERROR:
	default:
		mErrorCode = ERROR_UNKNOWN;
		break;
	}
}

std::string DOSException::getErrorMessage() const
{
	std::string msg = "DOS error: ";
	switch ( mErrorCode )
	{
		case ERROR_NO_ERROR:
			msg += "no error";
			break;
		case ERROR_FUNCTION_NOT_SUPPORTED:
			msg += "function not supported";
			break;
		case ERROR_FILE_NOT_FOUND:
			msg += "file does not exist";
			break;
		case ERROR_PATH_NOT_FOUND:
			msg += "directory does not exist";
			break;
		case ERROR_OUT_OF_HANDLES:
			msg += "too many open files";
			break;
		case ERROR_ACCESS_DENIED:
			msg += "access denied";
			break;
		case ERROR_INVALID_HANDLE:
			msg += "invalid handle";
			break;
		case ERROR_OUT_OF_MEMORY:
			msg += "out of memory";
			break;
		case ERROR_INVALID_DATA:
			msg += "invalid data";
			break;
		case ERROR_INVALID_DRIVE:
			msg += "invalid drive";
			break;
		case ERROR_GENERAL_FAILURE:
			msg += "general failure";
			break;
		default:
			msg += "unknown error";
			break;
	}
	return msg;
}

uint8_t DOSException::getErrorCode() const
{
	return mErrorCode;
}

uint8_t DOSException::getErrorClass() const
{
	return mErrorClass;
}

uint8_t DOSException::getRecommendedAction() const
{
	return mRecommendedAction;
}

uint8_t DOSException::getErrorLocus() const
{
	return mErrorLocus;
}
