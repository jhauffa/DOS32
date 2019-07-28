
#include "os/unix/UnixFile.h"
#include "os/unix/UnixMemMap.h"
#include "os/unix/UnixThread.h"
#include "os/unix/DarwinContext.h"
#include "os/unix/DarwinExceptionManager.h"
#include "os/unix/DarwinLDT.h"
#include "os/unix/UnixDateTime.h"
#include "os/unix/UnixPath.h"
#include "os/OS.h"


DarwinExceptionManager exceptMgr;


File *OS::createFile( const std::string &fileName, int access )
{
	return new UnixFile( fileName, access );
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

LDT *OS::createLDT()
{
	return new DarwinLDT();
}

Time *OS::createTime()
{
	return new UnixTime();
}

Path *OS::createPath( const std::string &hostPrefix, const std::string &dosSuffix )
{
	return new UnixPath( hostPrefix, dosSuffix );
}
