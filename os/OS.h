
#ifndef __DOS32_OS_OS_H__
#define __DOS32_OS_OS_H__

#include <string>

#include "File.h"
#include "MemMap.h"
#include "Thread.h"
#include "Context.h"
#include "ExceptionManager.h"
#include "Ldt.h"
#include "DateTime.h"


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

		static Ldt &getLdt();

		static Time *createTime();
		// static Date *createDate();
};

#endif  // __DOS32_OS_OS_H__
