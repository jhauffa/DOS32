
#include <cstdlib>

#include "os/OS.h"
#include "os/ExceptionInfo.h"
#include "Debug.h"
#include "Registers.h"
#include "DOS.h"
#include "DOSExtender.h"
#include "DPMI.h"
#include "InterruptHandler.h"
#include "ExecutionEnvironment.h"


PLACE_INSTANCE( ExecutionEnvironment );


ExecutionEnvironment::ExecutionEnvironment( int argc, char *argv[], char *envp[] ) :
	Singleton<ExecutionEnvironment>( this )
{
	mDOS = new DOS( argc, argv, envp );
	mDOSExtender = new DOSExtender( this, mDOS );
	mDPMI = new DPMI( this );
}

ExecutionEnvironment::~ExecutionEnvironment()
{
	delete mDPMI;
	delete mDOSExtender;
	delete mDOS;
}

void ExecutionEnvironment::registerInterruptHandler( uint8_t idx,
	InterruptHandler *handler )
{
	mInterruptHandlers[idx] = handler;
}

DescriptorTable &ExecutionEnvironment::getDescriptorTable()
{
	return mDescriptorTable;
}

int ExecutionEnvironment::run( Image *img )
{
	Thread &thr = *OS::createThread( appThreadProc, img );
	thr.run();
	return thr.join();
}

int ExecutionEnvironment::appThreadProc( void *data )
{
	ExceptionManager &mgr = OS::getExceptionManager();
	mgr.setMemoryExceptionHandler( memoryExceptionHandler );
	mgr.setConsoleInterruptHandler( consoleInterruptHandler );

	// run application
	getInstance().mDOSExtender->run( (Image *) data );
	return 0;  // should not happen
}

void ExecutionEnvironment::memoryExceptionHandler( ExceptionInfo &info )
{
	Context &ctx = info.getMutableContext();
	uint8_t *eip = (uint8_t *) ctx.getEIP();
	bool canResume = true;

	// parse instruction prefix
	int instrSize = decodePrefix( eip );
	eip += instrSize;

	// parse instruction
	switch ( eip[0] )
	{
		case 0xCD:
		{
			// interrupt
			InterruptHandler *hdl = getInstance().mInterruptHandlers[eip[1]];
			if ( !hdl )
			{
				FIXME( "interrupt 0x%02x, AX = 0x%04x\n", eip[1], ctx.getAX() );
				canResume = false;
			}
			else
				canResume = hdl->handleInterrupt( eip[1], ctx );
			instrSize += 2;
			break;
		}
		case 0xFA:
			FIXME( "clear interrupt flag\n" );
			instrSize += 1;
			break;
		case 0xFB:
			FIXME( "set interrupt flag\n" );
			instrSize += 1;
			break;
		default:
			canResume = false;
			break;
	}

	if ( !canResume )
	{
		info.dump();
		exit( 2 );
	}
	else
	{
		ctx.adjustEIP( instrSize );
//		TRACE( "continuing\n" );
	}
}

void ExecutionEnvironment::consoleInterruptHandler( ExceptionInfo &info )
{
	const Context &ctx = info.getContext();
	TRACE( "\nlast EIP = 0x%x\n", ctx.getEIP() );
	exit( 3 );
}

int ExecutionEnvironment::decodePrefix( uint8_t *data )
{
	int instrStart = 0;
	bool foundInstr = false;
	while ( !foundInstr )
	{
		switch ( data[instrStart] )
		{
			case 0x66:	// data size override
			case 0x67:	// address size override
			case 0x26:	// ES
			case 0x2E:	// CS
			case 0x36:	// SS
			case 0x3E:	// DS
			case 0x64:	// FS
			case 0x65:	// GS
			case 0xF0:	// LOCK
			case 0xF2:	// REPNE
			case 0xF3:	// REP
				break;
			default:
				foundInstr = true;
				break;
		}
		if ( !foundInstr )
			instrStart++;
	}
	return instrStart;
}
