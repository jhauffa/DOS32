
#ifndef __DOS32_IMAGE_LOADER_EXCEPTION_H__
#define __DOS32_IMAGE_LOADER_EXCEPTION_H__

#include "os/OsException.h"
#include "Exception.h"


class ImageLoaderException : public Exception
{
	public:

		#define NUM_LOADER_ERRORS 12
		enum LoaderError {
			IMG_NO_ERROR = 0,

			// specified file does not exist
			IMG_FILE_NOT_FOUND,

			// opening or reading from the file is not permitted
			IMG_ACCESS_DENIED,

			// any error that occurs on opening or reading the file and is not covered by
			// the other enums
			IMG_IO_ERROR,

			// not recoginzed as a valid image / image format unknown to the loader
			IMG_UNKNOWN_FORMAT,

			// loader recognized the format, but file contains invalid / inconsistent data
			// or is truncated
			IMG_CORRUPTED,

			// image cannot be loaded / executed due to limitations of the machine
			IMG_INCOMPATIBLE,

			// image cannot be loaded / executed, because the loader doesn't support a
			// certain feature
			IMG_NOT_SUPPORTED,

			IMG_OUT_OF_MEMORY,

			IMG_INTERNAL_ERROR,

			IMG_NOT_EXECUTABLE,

			IMG_NOT_RELOCATABLE
		};

		ImageLoaderException( LoaderError error );
		ImageLoaderException( const OsException &ex );

		std::string getErrorMessage() const;
		int getError() const;

	protected:
		LoaderError mError;

		static const char *mMessages[NUM_LOADER_ERRORS];

		static LoaderError osErrorToLoaderError( int error );
};


#endif  // __DOS32_IMAGE_LOADER_EXCEPTION_H__
