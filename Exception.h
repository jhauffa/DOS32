
#ifndef __DOS32_EXCEPTION_H__
#define __DOS32_EXCEPTION_H__

#include <string>


class Exception
{
	public:
		virtual ~Exception() {}

		virtual std::string getErrorMessage() const = 0;
		virtual int getError() const = 0;
};


#endif  // __DOS32_EXCEPTION_H__
