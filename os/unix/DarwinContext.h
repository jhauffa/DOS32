
#ifndef __DOS32_OS_UNIX_DARWIN_CONTEXT_H__
#define __DOS32_OS_UNIX_DARWIN_CONTEXT_H__

#include <signal.h>

#include "os/Context.h"


namespace host {

class DarwinContext : public Context
{
	public:
		DarwinContext( void *ctx );
		virtual ~DarwinContext() {}

		// AX
		virtual uint32_t getEAX() const;
		virtual void setEAX( uint32_t value );
		virtual uint16_t getAX() const;
		virtual void setAX( uint16_t value );
		virtual uint8_t getAH() const;
		virtual void setAH( uint8_t value );
		virtual uint8_t getAL() const;
		virtual void setAL( uint8_t value );

		// BX
		virtual uint32_t getEBX() const;
		virtual void setEBX( uint32_t value );
		virtual uint16_t getBX() const;
		virtual void setBX( uint16_t value );
		virtual uint8_t getBH() const;
		virtual void setBH( uint8_t value );
		virtual uint8_t getBL() const;
		virtual void setBL( uint8_t value );

		// CX
		virtual uint32_t getECX() const;
		virtual void setECX( uint32_t value );
		virtual uint16_t getCX() const;
		virtual void setCX( uint16_t value );
		virtual uint8_t getCH() const;
		virtual void setCH( uint8_t value );
		virtual uint8_t getCL() const;
		virtual void setCL( uint8_t value );

		// DX
		virtual uint32_t getEDX() const;
		virtual void setEDX( uint32_t value );
		virtual uint16_t getDX() const;
		virtual void setDX( uint16_t value );
		virtual uint8_t getDH() const;
		virtual void setDH( uint8_t value );
		virtual uint8_t getDL() const;
		virtual void setDL( uint8_t value );

		// SI
		virtual uint32_t getESI() const;
		virtual void setESI( uint32_t value );
		virtual uint16_t getSI() const;
		virtual void setSI( uint16_t value );

		// DI
		virtual uint32_t getEDI() const;
		virtual void setEDI( uint32_t value );
		virtual uint16_t getDI() const;
		virtual void setDI( uint16_t value );

		// SP
		virtual uint32_t getESP() const;
		virtual void setESP( uint32_t value );
		virtual uint16_t getSP() const;
		virtual void setSP( uint16_t value );

		// BP
		virtual uint32_t getEBP() const;
		virtual void setEBP( uint32_t value );
		virtual uint16_t getBP() const;
		virtual void setBP( uint16_t value );

		// EFLAGS
		virtual uint32_t getEFLAGS() const;
		virtual void setEFLAGS( uint32_t value );
		virtual bool getCF() const;
		virtual void setCF( bool value );
		virtual bool getZF() const;
		virtual void setZF( bool value );

		// EIP
		virtual uint32_t getEIP() const;
		virtual void setEIP( uint32_t value );
		virtual void adjustEIP( int32_t offset );

		// segment registers
		virtual uint16_t getSS() const;
		virtual void setSS( uint16_t value );
		virtual uint16_t getCS() const;
		virtual void setCS( uint16_t value );
		virtual uint16_t getDS() const;
		virtual void setDS( uint16_t value );
		virtual uint16_t getES() const;
		virtual void setES( uint16_t value );
		virtual uint16_t getFS() const;
		virtual void setFS( uint16_t value );
		virtual uint16_t getGS() const;
		virtual void setGS( uint16_t value );

	private:
		enum Flags { CF = 0x00000001, ZF = 0x00000040 };

		mcontext_t mContext;
};

}


#endif  // __DOS32_OS_UNIX_DARWIN_CONTEXT_H__
