
#include <unistd.h>

#include "os/unix/UnixFile.h"
#include "os/unix/UnixMemMap.h"
#include "os/unix/UnixThread.h"
#include "os/unix/DarwinContext.h"
#include "os/unix/DarwinExceptionManager.h"
#include "os/unix/DarwinLdt.h"
#include "os/unix/UnixDateTime.h"
#include "os/OS.h"


DarwinExceptionManager exceptMgr;


File *OS::createFile( const std::string &fileName, int access )
{
	return new UnixFile( fileName, access );
}

File *OS::createStdInFile()
{
	return new UnixFile( STDIN_FILENO );
}

File *OS::createStdOutFile()
{
	return new UnixFile( STDOUT_FILENO );
}

File *OS::createStdErrFile()
{
	return new UnixFile( STDERR_FILENO );
}

MemMap *OS::createMemMap( const File &file, int access )
{
	return new UnixMemMap( file, access );
}

MemMap *OS::createMemMap( MemSize size, int access )
{
	return new UnixMemMap( size, access );
}

Thread *OS::createThread( ThreadProc p, void *data )
{
	return new UnixThread( p, data );
}

void OS::exitThread( int result )
{
	UnixThread::exit( result );
}

Context *OS::createContext( void *ctx )
{
	return new DarwinContext( ctx );
}

ExceptionManager &OS::getExceptionManager()
{
	return DarwinExceptionManager::getInstance();
}

Ldt *OS::createLdt()
{
	return new DarwinLdt();
}

Time *OS::createTime()
{
	return new UnixTime();
}
