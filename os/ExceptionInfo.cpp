
#include <cstdio>

#include "os/ExceptionInfo.h"
#include "os/Context.h"


void ExceptionInfo::dump() const
{
	const Context &ctx = getContext();
	ctx.dump();

	uint8_t *eip = (uint8_t *) ctx.getEIP();
	if ( eip )
	{
		fprintf( stderr, "memory at EIP: " );
		for ( int i = 0; i < 8; i++ )
			fprintf( stderr, "%02x ", eip[i] );
		fprintf( stderr, "\n" );
	}
}
