
#ifndef __DOS32_OS_UNIX_EXCEPTION_H__
#define __DOS32_OS_UNIX_EXCEPTION_H__

#include "os/OsException.h"


class UnixException : public OsException
{
	public:
		UnixException();
		UnixException( int error );

		virtual std::string getErrorMessage() const;
		virtual int getError() const;

	private:
		int mError;
};


#endif  // __DOS32_OS_UNIX_EXCEPTION_H__
