
#include <stdlib.h>

#include "os/unix/DarwinContext.h"


DarwinContext::DarwinContext( void *ctx )
{
	mContext = ( (ucontext_t *) ctx )->uc_mcontext;
}


uint32_t DarwinContext::getEAX() const
{
	return mContext->__ss.__eax;
}

void DarwinContext::setEAX( uint32_t value )
{
	mContext->__ss.__eax = value;
}

uint16_t DarwinContext::getAX() const
{
	return mContext->__ss.__eax & 0xFFFF;
}

void DarwinContext::setAX( uint16_t value )
{
	mContext->__ss.__eax = ( mContext->__ss.__eax & 0xFFFF0000 ) | value;
}

uint8_t DarwinContext::getAH() const
{
	return ( mContext->__ss.__eax >> 8 ) & 0xFF;
}

void DarwinContext::setAH( uint8_t value )
{
	mContext->__ss.__eax = ( mContext->__ss.__eax & 0xFFFF00FF ) | ( value << 8 );
}

uint8_t DarwinContext::getAL() const
{
	return mContext->__ss.__eax & 0xFF;
}

void DarwinContext::setAL( uint8_t value )
{
	mContext->__ss.__eax = ( mContext->__ss.__eax & 0xFFFFFF00 ) | value;
}


uint32_t DarwinContext::getEBX() const
{
	return mContext->__ss.__ebx;
}

void DarwinContext::setEBX( uint32_t value )
{
	mContext->__ss.__ebx = value;
}

uint16_t DarwinContext::getBX() const
{
	return mContext->__ss.__ebx & 0xFFFF;
}

void DarwinContext::setBX( uint16_t value )
{
	mContext->__ss.__ebx = ( mContext->__ss.__ebx & 0xFFFF0000 ) | value;
}

uint8_t DarwinContext::getBH() const
{
	return ( mContext->__ss.__ebx >> 8 ) & 0xFF;
}

void DarwinContext::setBH( uint8_t value )
{
	mContext->__ss.__ebx = ( mContext->__ss.__ebx & 0xFFFF00FF ) | ( value << 8 );
}

uint8_t DarwinContext::getBL() const
{
	return mContext->__ss.__ebx & 0xFF;
}

void DarwinContext::setBL( uint8_t value )
{
	mContext->__ss.__ebx = ( mContext->__ss.__ebx & 0xFFFFFF00 ) | value;
}


uint32_t DarwinContext::getECX() const
{
	return mContext->__ss.__ecx;
}

void DarwinContext::setECX( uint32_t value )
{
	mContext->__ss.__ecx = value;
}

uint16_t DarwinContext::getCX() const
{
	return mContext->__ss.__ecx & 0xFFFF;
}

void DarwinContext::setCX( uint16_t value )
{
	mContext->__ss.__ecx = ( mContext->__ss.__ecx & 0xFFFF0000 ) | value;
}

uint8_t DarwinContext::getCH() const
{
	return ( mContext->__ss.__ecx >> 8 ) & 0xFF;
}

void DarwinContext::setCH( uint8_t value )
{
	mContext->__ss.__ecx = ( mContext->__ss.__ecx & 0xFFFF00FF ) | ( value << 8 );
}

uint8_t DarwinContext::getCL() const
{
	return mContext->__ss.__ecx & 0xFF;
}

void DarwinContext::setCL( uint8_t value )
{
	mContext->__ss.__ecx = ( mContext->__ss.__ecx & 0xFFFFFF00 ) | value;
}


uint32_t DarwinContext::getEDX() const
{
	return mContext->__ss.__edx;
}

void DarwinContext::setEDX( uint32_t value )
{
	mContext->__ss.__edx = value;
}

uint16_t DarwinContext::getDX() const
{
	return mContext->__ss.__edx & 0xFFFF;
}

void DarwinContext::setDX( uint16_t value )
{
	mContext->__ss.__edx = ( mContext->__ss.__edx & 0xFFFF0000 ) | value;
}

uint8_t DarwinContext::getDH() const
{
	return ( mContext->__ss.__edx >> 8 ) & 0xFF;
}

void DarwinContext::setDH( uint8_t value )
{
	mContext->__ss.__edx = ( mContext->__ss.__edx & 0xFFFF00FF ) | ( value << 8 );
}

uint8_t DarwinContext::getDL() const
{
	return mContext->__ss.__edx & 0xFF;
}

void DarwinContext::setDL( uint8_t value )
{
	mContext->__ss.__edx = ( mContext->__ss.__edx & 0xFFFFFF00 ) | value;
}


uint32_t DarwinContext::getESI() const
{
	return mContext->__ss.__esi;
}

void DarwinContext::setESI( uint32_t value )
{
	mContext->__ss.__esi = value;
}

uint16_t DarwinContext::getSI() const
{
	return mContext->__ss.__esi & 0xFFFF;
}

void DarwinContext::setSI( uint16_t value )
{
	mContext->__ss.__esi = ( mContext->__ss.__esi & 0xFFFF0000 ) | value;
}


uint32_t DarwinContext::getEDI() const
{
	return mContext->__ss.__edi;
}

void DarwinContext::setEDI( uint32_t value )
{
	mContext->__ss.__edi = value;
}

uint16_t DarwinContext::getDI() const
{
	return mContext->__ss.__edi & 0xFFFF;
}

void DarwinContext::setDI( uint16_t value )
{
	mContext->__ss.__edi = ( mContext->__ss.__edi & 0xFFFF0000 ) | value;
}


uint32_t DarwinContext::getESP() const
{
	return mContext->__ss.__esp;
}

void DarwinContext::setESP( uint32_t value )
{
	mContext->__ss.__esp = value;
}

uint16_t DarwinContext::getSP() const
{
	return mContext->__ss.__esp & 0xFFFF;
}

void DarwinContext::setSP( uint16_t value )
{
	mContext->__ss.__esp = ( mContext->__ss.__esp & 0xFFFF0000 ) | value;
}


uint32_t DarwinContext::getEBP() const
{
	return mContext->__ss.__ebp;
}

void DarwinContext::setEBP( uint32_t value )
{
	mContext->__ss.__ebp = value;
}

uint16_t DarwinContext::getBP() const
{
	return mContext->__ss.__ebp & 0xFFFF;
}

void DarwinContext::setBP( uint16_t value )
{
	mContext->__ss.__ebp = ( mContext->__ss.__ebp & 0xFFFF0000 ) | value;
}


uint32_t DarwinContext::getEFLAGS() const
{
	return mContext->__ss.__eflags;
}

void DarwinContext::setEFLAGS( uint32_t value )
{
	mContext->__ss.__eflags = value;
}

bool DarwinContext::getCF() const
{
	return ( mContext->__ss.__eflags & CF );
}

void DarwinContext::setCF( bool value )
{
	if ( value )
		mContext->__ss.__eflags |= CF;
	else
		mContext->__ss.__eflags &= ~CF;
}

bool DarwinContext::getZF() const
{
	return ( mContext->__ss.__eflags & ZF );
}

void DarwinContext::setZF( bool value )
{
	if ( value )
		mContext->__ss.__eflags |= ZF;
	else
		mContext->__ss.__eflags &= ~ZF;
}


uint32_t DarwinContext::getEIP() const
{
	return mContext->__ss.__eip;
}

void DarwinContext::setEIP( uint32_t value )
{
	mContext->__ss.__eip = value;
}

void DarwinContext::adjustEIP( int32_t offset )
{
	mContext->__ss.__eip += offset;
}


uint16_t DarwinContext::getSS() const
{
	return mContext->__ss.__ss & 0xFFFF;
}

void DarwinContext::setSS( uint16_t value )
{
	mContext->__ss.__ss = ( mContext->__ss.__ss & 0xFFFF0000 ) | value;
}

uint16_t DarwinContext::getCS() const
{
	return mContext->__ss.__cs & 0xFFFF;
}

void DarwinContext::setCS( uint16_t value )
{
	mContext->__ss.__cs = ( mContext->__ss.__cs & 0xFFFF0000 ) | value;
}

uint16_t DarwinContext::getDS() const
{
	return mContext->__ss.__ds & 0xFFFF;
}

void DarwinContext::setDS( uint16_t value )
{
	mContext->__ss.__ds = ( mContext->__ss.__ds & 0xFFFF0000 ) | value;
}

uint16_t DarwinContext::getES() const
{
	return mContext->__ss.__es & 0xFFFF;
}

void DarwinContext::setES( uint16_t value )
{
	mContext->__ss.__es = ( mContext->__ss.__es & 0xFFFF0000 ) | value;
}

uint16_t DarwinContext::getFS() const
{
	return mContext->__ss.__fs & 0xFFFF;
}

void DarwinContext::setFS( uint16_t value )
{
	mContext->__ss.__fs = ( mContext->__ss.__fs & 0xFFFF0000 ) | value;
}

uint16_t DarwinContext::getGS() const
{
	return mContext->__ss.__gs & 0xFFFF;
}

void DarwinContext::setGS( uint16_t value )
{
	mContext->__ss.__gs = ( mContext->__ss.__gs & 0xFFFF0000 ) | value;
}
