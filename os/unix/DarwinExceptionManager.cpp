
#include "Debug.h"
#include "os/OS.h"
#include "os/unix/UnixMemMap.h"
#include "os/unix/UnixException.h"
#include "os/unix/DarwinExceptionInfo.h"
#include "os/unix/DarwinExceptionManager.h"

#define CANARY  0xCAF00F00


DEFINE_INSTANCE( DarwinExceptionManager );

uint32_t DarwinExceptionManager::mReenterCount = 0;


DarwinExceptionManager::DarwinExceptionManager() :
	Singleton<DarwinExceptionManager>( this ),
	mMemoryExceptionHandler( NULL ), mConsoleInterruptHandler( NULL )
{
	// allocate signal stack
	mStack = new UnixMemMap( SIGSTKSZ, MemMap::ACC_READ | MemMap::ACC_WRITE );
	mStackBottom = (uint32_t *) mStack->getPtr();
	TRACE( "stack at %p\n", mStackBottom );
	*mStackBottom = CANARY;

	stack_t sigStack;
	sigStack.ss_sp = mStackBottom;
	sigStack.ss_size = SIGSTKSZ;
	sigStack.ss_flags = 0;
	if ( sigaltstack( &sigStack, NULL ) == -1 )
		throw UnixException();

	// install signal handlers
	struct sigaction act;
	act.sa_sigaction = signalHandler;
	act.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER;
	act.sa_mask = 0;

	sigaction( SIGBUS, &act, NULL );
	sigaction( SIGSEGV, &act, NULL );

	act.sa_flags &= ~SA_NODEFER;

	sigaction( SIGINT, &act, NULL );
}

DarwinExceptionManager::~DarwinExceptionManager()
{
	delete mStack;
}

void DarwinExceptionManager::setMemoryExceptionHandler( ExceptionHandler handler )
{
	mMemoryExceptionHandler = handler;
}

void DarwinExceptionManager::setConsoleInterruptHandler( ExceptionHandler handler )
{
	mConsoleInterruptHandler = handler;
}

void DarwinExceptionManager::signalHandler( int sig, siginfo_t *info, void *data )
{
	DarwinExceptionInfo exc( sig, info, data );

	if ( ++mReenterCount > 1 )
	{
		ERR( "\n\nbad memory access in emulator code\n" );
		exc.dump();
		exit( 3 );
	}

	DarwinExceptionManager &inst = getInstance();
	ExceptionHandler handler = NULL;

	switch ( sig )
	{
		case SIGBUS:
		case SIGSEGV:
			handler = inst.mMemoryExceptionHandler;
			break;
		case SIGINT:
			handler = inst.mConsoleInterruptHandler;
			break;
	}

	if ( handler )
	{
		handler( exc );
		if ( *inst.mStackBottom != CANARY )
		{
			ERR( "signal stack overrun\n" );
			exit( 3 );
		}
	}
	else
	{
		ERR( "unexpected signal %d\n", sig );
		exit( 3 );
	}
	mReenterCount--;
}
