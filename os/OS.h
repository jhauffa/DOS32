
#ifndef __DOS32_OS_OS_H__
#define __DOS32_OS_OS_H__

#include <string>

#include "os/MemMap.h"
#include "os/Thread.h"


namespace host {

class File;
class Context;
class ExceptionManager;
class LDT;
class Time;
class Path;


class OS
{
	public:
		static File *createFile( const std::string &fileName, int access );

		static MemMap *createMemMap( const File &file, int access );
		static MemMap *createMemMap( MemSize size, int access );

		static Thread *createThread( ThreadProc p, void *data );
		static void exitThread( int result );

		static Context *createContext( void *ctx );

		static ExceptionManager &getExceptionManager();

		static LDT *createLDT();

		static Time *createTime();

		static Path *createPath( const std::string &hostPrefix,
			const std::string &dosSuffix );
};

}


#endif  // __DOS32_OS_OS_H__
