
#ifndef __DOS32_OS_UNIX_DARWIN_EXCEPTION_MANAGER__
#define __DOS32_OS_UNIX_DARWIN_EXCEPTION_MANAGER__

#include <map>
#include <signal.h>
#include <stdint.h>

#include "Singleton.h"
#include "os/ExceptionManager.h"


class UnixMemMap;

class DarwinExceptionManager : public ExceptionManager,
	public Singleton<DarwinExceptionManager>
{
	public:
		DarwinExceptionManager();
		virtual ~DarwinExceptionManager();

		virtual void setMemoryExceptionHandler( ExceptionHandler handler );
		virtual void setConsoleInterruptHandler( ExceptionHandler handler );

	private:
		UnixMemMap *mStack;
		uint32_t *mStackBottom;
		ExceptionHandler mMemoryExceptionHandler;
		ExceptionHandler mConsoleInterruptHandler;

		static uint32_t mReenterCount;

		static void signalHandler( int sig, siginfo_t *info, void *data );
};


#endif  // __DOS32_OS_UNIX_DARWIN_EXCEPTION_MANAGER__
