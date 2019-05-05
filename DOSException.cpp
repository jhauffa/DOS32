
#include "DOSException.h"


DOSException::DOSException( uint8_t errorCode ) : mErrorCode ( errorCode ),
	mErrorClass( CLASS_UNKNOWN ), mRecommendedAction( ACTION_CLEANUP_ABORT ),
	mErrorLocus( LOCUS_UNKNOWN )
{
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
		case ERROR_OUT_OF_HANDLES:
			msg += "too many open files";
			break;
		case ERROR_INVALID_HANDLE:
			msg += "invalid handle";
			break;
		case ERROR_INVALID_DRIVE:
			msg += "invalid drive";
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
