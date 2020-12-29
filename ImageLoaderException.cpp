
#include "Debug.h"
#include "ImageLoaderException.h"


const char *ImageLoaderException::mMessages[NUM_LOADER_ERRORS] = {
	NULL,
	"file not found",
	"access denied",
	"read error",
	"format not recognized",
	"file is corrupted or truncated",
	"file cannot be executed on this machine",
	"file requires a function that is not yet implemented",
	"out of memory",
	"internal error",
	"file is not an application",
	"file does not contain any relocaton information"
};


ImageLoaderException::ImageLoaderException( LoaderError error ) : mError( error )
{
}

ImageLoaderException::ImageLoaderException( const host::OSException &ex )
{
	mError = osErrorToLoaderError( ex.getError() );
}

std::string ImageLoaderException::getErrorMessage() const
{
	assert( ( mError > 0 ) && ( mError < NUM_LOADER_ERRORS ) );
	return std::string( "error loading file: " ) + mMessages[mError];
}

int ImageLoaderException::getError() const
{
	return mError;
}

ImageLoaderException::LoaderError ImageLoaderException::osErrorToLoaderError( int error )
{
	switch ( error )
	{
		case host::OSException::OS_NO_ERROR:
			return IMG_NO_ERROR;
		case host::OSException::OS_ACCESS_DENIED:
			return IMG_ACCESS_DENIED;
		case host::OSException::OS_IO_ERROR:
			return IMG_IO_ERROR;
		case host::OSException::OS_FILE_NOT_FOUND:
			return IMG_FILE_NOT_FOUND;
		case host::OSException::OS_OUT_OF_MEMORY:
			return IMG_OUT_OF_MEMORY;
		default:
			return IMG_INTERNAL_ERROR;
	}
}
