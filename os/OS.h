
#ifndef __DOS32_OS_OS_H__
#define __DOS32_OS_OS_H__

#include <string>

#include "os/File.h"
#include "os/MemMap.h"
#include "os/Thread.h"
#include "os/Context.h"
#include "os/ExceptionManager.h"
#include "os/Ldt.h"
#include "os/DateTime.h"


class OS
{
	public:
		static File *createFile( const std::string &fileName, int access );
		static File *createStdInFile();
		static File *createStdOutFile();
		static File *createStdErrFile();

		static MemMap *createMemMap( const File &file, int access );
		static MemMap *createMemMap( MemSize size, int access );

		static Thread *createThread( ThreadProc p, void *data );
		static void exitThread( int result );

		static Context *createContext( void *ctx );

		static ExceptionManager &getExceptionManager();

		static Ldt *createLdt();

		static Time *createTime();
};

#endif  // __DOS32_OS_OS_H__
