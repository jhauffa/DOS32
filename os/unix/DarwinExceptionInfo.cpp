
#include <cstdio>

#include "os/unix/DarwinContext.h"
#include "os/unix/DarwinExceptionInfo.h"


namespace host {

DarwinExceptionInfo::DarwinExceptionInfo( int sig, siginfo_t *info, void *data ) :
	mSignal( sig ), mInfo( info )
{
	mContext = new DarwinContext( data );
}

DarwinExceptionInfo::~DarwinExceptionInfo()
{
	delete mContext;
}

void *DarwinExceptionInfo::getFaultAddr() const
{
	return mInfo->si_addr;
}

const Context &DarwinExceptionInfo::getContext() const
{
	return *mContext;
}

Context &DarwinExceptionInfo::getMutableContext()
{
	return *mContext;
}

void DarwinExceptionInfo::dump() const
{
	const char *sigName = "unknown";
	const char *codeExp = sigName;
	if ( mSignal == SIGSEGV )
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
	else if ( mSignal == SIGBUS )
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
	fprintf( stderr, "\nsignal %s (%d), %s (code %d)\n", sigName, mSignal, codeExp,
		mInfo->si_code );
	fprintf( stderr, "fault address = %p\n", mInfo->si_addr );

	ExceptionInfo::dump();
}

}
