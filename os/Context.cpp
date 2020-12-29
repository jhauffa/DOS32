
#include <cstdio>

#include "os/Context.h"


namespace host {

void Context::dump() const
{
	fprintf( stderr, "EAX = 0x%08x  EBX = 0x%08x  ECX = 0x%08x  EDX = 0x%08x\n",
		getEAX(), getEBX(), getECX(), getEDX() );
	fprintf( stderr, "ESI = 0x%08x  EDI = 0x%08x  ESP = 0x%08x  EBP = 0x%08x\n",
		getESI(), getEDI(), getESP(), getEBP() );
	fprintf( stderr, "EIP = 0x%08x  EFLAGS = 0x%08x\n",
		getEIP(), getEFLAGS() );
	fprintf( stderr, "CS = 0x%04x  SS = 0x%04x  DS = 0x%04x  ES = 0x%04x\n" \
		"FS = 0x%04x  GS = 0x%04x\n",
		getCS(), getSS(), getDS(), getES(), getFS(), getGS() );
}

}
