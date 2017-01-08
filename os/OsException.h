
#ifndef __DOS32_OS_EXCEPTION_H__
#define __DOS32_OS_EXCEPTION_H__

#include "Exception.h"


class OSException : public Exception
{
	public:
		enum OSError {
			OS_NO_ERROR = 0, OS_ACCESS_DENIED, OS_INVALID_PARAMETER,
			OS_IO_ERROR, OS_FILE_NOT_FOUND, OS_OUT_OF_MEMORY, OS_BAD_ADDRESS,
			OS_UNKNOWN_ERROR
		};

		virtual ~OSException() {}
};


#endif  // __DOS32_OS_EXCEPTION_H__
