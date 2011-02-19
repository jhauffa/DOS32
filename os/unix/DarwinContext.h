
#ifndef __DOS32_OS_UNIX_DARWIN_CONTEXT_H__
#define __DOS32_OS_UNIX_DARWIN_CONTEXT_H__

#include <signal.h>

#include "os/Context.h"


class DarwinContext : public Context
{
	public:
		DarwinContext( void *ctx );

		// AX
		virtual uint32_t getEAX();
		virtual void setEAX( uint32_t value );
		virtual uint16_t getAX();
		virtual void setAX( uint16_t value );
		virtual uint8_t getAH();
		virtual void setAH( uint8_t value );
		virtual uint8_t getAL();
		virtual void setAL( uint8_t value );

		// BX
		virtual uint32_t getEBX();
		virtual void setEBX( uint32_t value );
		virtual uint16_t getBX();
		virtual void setBX( uint16_t value );
		virtual uint8_t getBH();
		virtual void setBH( uint8_t value );
		virtual uint8_t getBL();
		virtual void setBL( uint8_t value );

		// CX
		virtual uint32_t getECX();
		virtual void setECX( uint32_t value );
		virtual uint16_t getCX();
		virtual void setCX( uint16_t value );
		virtual uint8_t getCH();
		virtual void setCH( uint8_t value );
		virtual uint8_t getCL();
		virtual void setCL( uint8_t value );

		// DX
		virtual uint32_t getEDX();
		virtual void setEDX( uint32_t value );
		virtual uint16_t getDX();
		virtual void setDX( uint16_t value );
		virtual uint8_t getDH();
		virtual void setDH( uint8_t value );
		virtual uint8_t getDL();
		virtual void setDL( uint8_t value );

		// SI
		virtual uint32_t getESI();
		virtual void setESI( uint32_t value );
		virtual uint16_t getSI();
		virtual void setSI( uint16_t value );

		// DI
		virtual uint32_t getEDI();
		virtual void setEDI( uint32_t value );
		virtual uint16_t getDI();
		virtual void setDI( uint16_t value );

		// SP
		virtual uint32_t getESP();
		virtual void setESP( uint32_t value );
		virtual uint16_t getSP();
		virtual void setSP( uint16_t value );

		// BP
		virtual uint32_t getEBP();
		virtual void setEBP( uint32_t value );
		virtual uint16_t getBP();
		virtual void setBP( uint16_t value );

		// EFLAGS
		virtual uint32_t getEFLAGS();
		virtual void setEFLAGS( uint32_t value );
		virtual bool getCF();
		virtual void setCF( bool value );
		virtual bool getZF();
		virtual void setZF( bool value );

		// EIP
		virtual uint32_t getEIP();
		virtual void setEIP( uint32_t value );
		virtual void adjustEIP( int32_t offset );

		// segment registers
		virtual uint16_t getSS();
		virtual void setSS( uint16_t value );
		virtual uint16_t getCS();
		virtual void setCS( uint16_t value );
		virtual uint16_t getDS();
		virtual void setDS( uint16_t value );
		virtual uint16_t getES();
		virtual void setES( uint16_t value );
		virtual uint16_t getFS();
		virtual void setFS( uint16_t value );
		virtual uint16_t getGS();
		virtual void setGS( uint16_t value );

	private:
		enum Flags { CF = 0x00000001, ZF = 0x00000040 };

		mcontext_t mContext;
};


#endif  // __DOS32_OS_UNIX_DARWIN_CONTEXT_H__
