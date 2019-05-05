
#ifndef __DOS32_OS_OS_H__
#define __DOS32_OS_OS_H__

#include <string>

#include "os/MemMap.h"
#include "os/Thread.h"
#include "os/Context.h"
#include "os/ExceptionManager.h"
#include "os/LDT.h"
#include "os/DateTime.h"


class OS
{
	public:
		static MemMap *createMemMap( const std::string &fileName, int access );
		static MemMap *createMemMap( MemSize size, int access );

		static Thread *createThread( ThreadProc p, void *data );
		static void exitThread( int result );

		static Context *createContext( void *ctx );

		static ExceptionManager &getExceptionManager();

		static LDT *createLDT();

		static Time *createTime();
};

#endif  // __DOS32_OS_OS_H__
