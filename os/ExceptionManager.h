
#ifndef __DOS32_OS_EXCEPTION_MANAGER__
#define __DOS32_OS_EXCEPTION_MANAGER__

class ExceptionInfo;


typedef void (*ExceptionHandler)( ExceptionInfo & );


class ExceptionManager
{
	public:
		virtual ~ExceptionManager() {}

		virtual void setMemoryExceptionHandler( ExceptionHandler handler ) = 0;
		virtual void setConsoleInterruptHandler( ExceptionHandler handler ) = 0;
};


#endif  // __DOS32_OS_EXCEPTION_MANAGER__
