
#include "os/unix/UnixMemMap.h"
#include "os/unix/UnixThread.h"
#include "os/unix/DarwinContext.h"
#include "os/unix/DarwinExceptionManager.h"
#include "os/unix/DarwinLdt.h"
#include "os/unix/UnixDateTime.h"
#include "os/OS.h"


DarwinExceptionManager exceptMgr;


MemMap *OS::createMemMap( const std::string &fileName, int access )
{
	return new UnixMemMap( fileName, access );
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
