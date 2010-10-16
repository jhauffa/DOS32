
#include <stdio.h>

#include "../Debug.h"
#include "../Registers.h"
#include "Context.h"


uint32_t Context::getReg32( int idx )
{
	switch ( idx )
	{
		case REG_EAX:
			return getEAX();
		case REG_ECX:
			return getECX();
		case REG_EDX:
			return getEDX();
		case REG_EBX:
			return getEBX();
		case REG_ESP:
			return getESP();
		case REG_EBP:
			return getEBP();
		case REG_ESI:
			return getESI();
		case REG_EDI:
			return getEDI();
	}
	assert( false );
}

void Context::setReg32( int idx, uint32_t value )
{
	switch ( idx )
	{
		case REG_EAX:
			setEAX( value );
			break;
		case REG_ECX:
			setECX( value );
			break;
		case REG_EDX:
			setEDX( value );
			break;
		case REG_EBX:
			setEBX( value );
			break;
		case REG_ESP:
			setESP( value );
			break;
		case REG_EBP:
			setEBP( value );
			break;
		case REG_ESI:
			setESI( value );
			break;
		case REG_EDI:
			setEDI( value );
			break;
	}
}

uint16_t Context::getSegReg( int idx )
{
	switch ( idx )
	{
		case SEG_ES:
			return getES();
		case SEG_CS:
			return getCS();
		case SEG_SS:
			return getSS();
		case SEG_DS:
			return getDS();
		case SEG_FS:
			return getFS();
		case SEG_GS:
			return getGS();
	}
	assert( false );
}

void Context::setSegReg( int idx, uint16_t value )
{
	switch ( idx )
	{
		case SEG_ES:
			setES( value );
			break;
		case SEG_CS:
			setCS( value );
			break;
		case SEG_SS:
			setSS( value );
			break;
		case SEG_DS:
			setDS( value );
			break;
		case SEG_FS:
			setFS( value );
			break;
		case SEG_GS:
			setGS( value );
			break;
	}
}

void Context::dump()
{
	fprintf( stderr, "EAX = 0x%08x  EBX = 0x%08x  ECX = 0x%08x  EDX = 0x%08x\n",
		getEAX(), getEBX(), getECX(), getEDX() );
	fprintf( stderr, "ESI = 0x%08x  EDI = 0x%08x  ESP = 0x%08x  EBP = 0x%08x\n",
		getESI(), getEDI(), getESP(), getEBP() );
	fprintf( stderr, "EIP = 0x%08x  EFLAGS = 0x%08x\n",
		getEIP(), getEFLAGS() );
}
