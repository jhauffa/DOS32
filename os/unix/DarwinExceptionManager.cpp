
#include "Debug.h"
#include "os/OS.h"
#include "os/unix/UnixMemMap.h"
#include "os/unix/UnixException.h"
#include "os/unix/DarwinExceptionInfo.h"
#include "os/unix/DarwinExceptionManager.h"

namespace host {

#define CANARY  0xCAF00F00


PLACE_INSTANCE( DarwinExceptionManager );

uint32_t DarwinExceptionManager::mReenterCount = 0;
uint16_t DarwinExceptionManager::mOSCodeSel = 0;
uint16_t DarwinExceptionManager::mOSDataSel = 0;
uint16_t DarwinExceptionManager::mOSThreadSel = 0;
UnixMemMap *DarwinExceptionManager::mRestoreSegRegsCode = NULL;


DarwinExceptionManager::DarwinExceptionManager() :
	Singleton<DarwinExceptionManager>( this ),
	mMemoryExceptionHandler( NULL ), mConsoleInterruptHandler( NULL )
{
	// save code and data selectors provided by OS
	asm ( "mov %%cs, %%ax\n\t" : "=a" (mOSCodeSel) );
	asm ( "mov %%ds, %%ax\n\t" : "=a" (mOSDataSel) );
	asm ( "mov %%gs, %%ax\n\t" : "=a" (mOSThreadSel) );

	// allocate space for code restoring the segment registers after leaving the signal
	// handler
	mRestoreSegRegsCode = new UnixMemMap( 4096,
		MemMap::ACC_READ | MemMap::ACC_WRITE | MemMap::ACC_EXEC );

	// allocate signal stack
	mStack = new UnixMemMap( SIGSTKSZ, MemMap::ACC_READ | MemMap::ACC_WRITE );
	mStackBottom = reinterpret_cast<uint32_t *>( mStack->getPtr() );
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

void DarwinExceptionManager::emitLoadAX( uint8_t *&buf, uint16_t value )
{
	// MOV AX, value
	*buf++ = 0x66;
	*buf++ = 0xb8;
	*reinterpret_cast<uint16_t *>( buf ) = value;
	buf += 2;
}

void DarwinExceptionManager::emitLoadSegReg( uint8_t *&buf, uint8_t segCode )
{
	// MOV xS, AX
	*buf++ = 0x8e;
	*buf++ = segCode;
}

void DarwinExceptionManager::emitFarJump( uint8_t *&buf, uint16_t sel, uint32_t addr )
{
	// JMP FAR sel:addr
	*buf++ = 0xea;
	*reinterpret_cast<uint32_t *>( buf ) = addr;
	buf += 4;
	*reinterpret_cast<uint16_t *>( buf ) = sel;
	buf += 2;
}

void DarwinExceptionManager::emitNearJump( uint8_t *&buf, uint32_t addr )
{
	// JMP addr (relative to EIP after current instruction)
	*buf++ = 0xe9;
	*reinterpret_cast<uint32_t *>( buf ) = addr -
		( reinterpret_cast<uint32_t>( buf ) + 4 );
	buf += 4;
}

void DarwinExceptionManager::signalHandler( int sig, siginfo_t *info, void *data )
{
	DarwinExceptionInfo exc( sig, info, data );
	Context &ctx = exc.getMutableContext();

	if ( ( ++mReenterCount > 1 ) ||
	     mRestoreSegRegsCode->isInRange( reinterpret_cast<void *>( ctx.getEIP() ) ) )
	{
		ERR( "bad memory access in emulator code\n" );
		exc.dump();
		exit( 3 );
	}

	ExceptionHandler handler = NULL;
	DarwinExceptionManager &inst = getInstance();
	switch ( sig )
	{
		case SIGBUS:
		case SIGSEGV:
			handler = inst.mMemoryExceptionHandler;
			break;
		case SIGINT:
			handler = inst.mConsoleInterruptHandler;
			break;
		default:
			ERR( "unexpected signal %d\n", sig );
			exit( 3 );
	}

	handler( exc );
	if ( *inst.mStackBottom != CANARY )
	{
		ERR( "signal stack overrun\n" );
		exit( 3 );
	}

	/* MacOS preserves the content of the segment registers on context switches, but
	   resets them to default values on return from a signal handler (see
	   set_thread_state32 in osfmk/i386/pcb.c of the XNU source code). Since loading a
	   segment register is expensive, we build a custom piece of code that specifically
	   restores those segment registers that have been modified by the DOS program. */
	uint8_t *bufBase = reinterpret_cast<uint8_t *>( mRestoreSegRegsCode->getPtr() );
	uint8_t *buf = bufBase;
	if ( ctx.getSS() != mOSDataSel )
	{
		emitLoadAX( buf, ctx.getSS() );
		emitLoadSegReg( buf, 0xd0 );
	}
	if ( ctx.getDS() != mOSDataSel )
	{
		emitLoadAX( buf, ctx.getDS() );
		emitLoadSegReg( buf, 0xd8 );
	}
	if ( ctx.getES() != mOSDataSel )
	{
		emitLoadAX( buf, ctx.getES() );
		emitLoadSegReg( buf, 0xc0 );
	}
	// FS is left alone for whatever reason
	if ( ctx.getGS() != mOSThreadSel )
	{
		emitLoadAX( buf, ctx.getGS() );
		emitLoadSegReg( buf, 0xe8 );
	}
	if ( buf > bufBase )
		emitLoadAX( buf, ctx.getAX() );
	if ( ctx.getCS() != mOSCodeSel )
		emitFarJump( buf, ctx.getCS(), ctx.getEIP() );
	else if ( buf > bufBase )
		emitNearJump( buf, ctx.getEIP() );
	if ( buf > bufBase )
		ctx.setEIP( reinterpret_cast<uint32_t>( bufBase ) );

	mReenterCount--;
}

}
