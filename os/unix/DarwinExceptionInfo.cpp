
#include <stdio.h>

#include "DarwinContext.h"
#include "DarwinExceptionInfo.h"


DarwinExceptionInfo::DarwinExceptionInfo( int sig, siginfo_t *info, void *data ) :
	mSignal( sig ), mInfo( info )
{
	mContext = new DarwinContext( data );
}

DarwinExceptionInfo::~DarwinExceptionInfo()
{
	delete mContext;
}

void *DarwinExceptionInfo::getFaultAddr()
{
	return mInfo->si_addr;
}

Context &DarwinExceptionInfo::getContext()
{
	return *mContext;
}

void DarwinExceptionInfo::dump()
{
	const char *sigName;
	const char *codeExp;
	getSignalName( mSignal, mInfo->si_code, sigName, codeExp );

	fprintf( stderr, "\nsignal %s (%d), %s (code %d)\n", sigName, mSignal, codeExp,
		mInfo->si_code );
	fprintf( stderr, "fault address = %p\n", mInfo->si_addr );

	mContext->dump();

	uint8_t *eip = (uint8_t *) mContext->getEIP();
	if ( eip )
	{
		fprintf( stderr, "memory at EIP: " );
		for ( int i = 0; i < 8; i++ )
			fprintf( stderr, "%02x ", eip[i] );
		fprintf( stderr, "\n" );
	}
}

void DarwinExceptionInfo::getSignalName( int sig, int code, const char *&sigName,
	const char *&codeExp )
{
	sigName = "unknown";
	codeExp = sigName;
	if ( sig == SIGSEGV )
	{
		sigName = "SIGSEGV";
		switch ( mInfo->si_code )
		{
			case SEGV_MAPERR:
				codeExp = "address not mapped";
				break;
			case SEGV_ACCERR:
				codeExp = "access not permitted";
				break;
		}
	}
	else if ( sig == SIGBUS )
	{
		sigName = "SIGBUS";
		switch ( mInfo->si_code )
		{
			case BUS_ADRALN:
				codeExp = "unaligned access";
				break;
			case BUS_ADRERR:
				codeExp = "invalid physical address";
				break;
			case BUS_OBJERR:
				codeExp = "HW specific error";
				break;
		}
	}
}
