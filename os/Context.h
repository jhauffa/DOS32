
#ifndef __DOS32_OS_CONTEXT_H__
#define __DOS32_OS_CONTEXT_H__

#include <stdint.h>


class Context
{
	public:
		virtual ~Context() {}

		// AX
		virtual uint32_t getEAX() = 0;
		virtual void setEAX( uint32_t value ) = 0;
		virtual uint16_t getAX() = 0;
		virtual void setAX( uint16_t value ) = 0;
		virtual uint8_t getAH() = 0;
		virtual void setAH( uint8_t value ) = 0;
		virtual uint8_t getAL() = 0;
		virtual void setAL( uint8_t value ) = 0;

		// BX
		virtual uint32_t getEBX() = 0;
		virtual void setEBX( uint32_t value ) = 0;
		virtual uint16_t getBX() = 0;
		virtual void setBX( uint16_t value ) = 0;
		virtual uint8_t getBH() = 0;
		virtual void setBH( uint8_t value ) = 0;
		virtual uint8_t getBL() = 0;
		virtual void setBL( uint8_t value ) = 0;

		// CX
		virtual uint32_t getECX() = 0;
		virtual void setECX( uint32_t value ) = 0;
		virtual uint16_t getCX() = 0;
		virtual void setCX( uint16_t value ) = 0;
		virtual uint8_t getCH() = 0;
		virtual void setCH( uint8_t value ) = 0;
		virtual uint8_t getCL() = 0;
		virtual void setCL( uint8_t value ) = 0;

		// DX
		virtual uint32_t getEDX() = 0;
		virtual void setEDX( uint32_t value ) = 0;
		virtual uint16_t getDX() = 0;
		virtual void setDX( uint16_t value ) = 0;
		virtual uint8_t getDH() = 0;
		virtual void setDH( uint8_t value ) = 0;
		virtual uint8_t getDL() = 0;
		virtual void setDL( uint8_t value ) = 0;

		// SI
		virtual uint32_t getESI() = 0;
		virtual void setESI( uint32_t value ) = 0;
		virtual uint16_t getSI() = 0;
		virtual void setSI( uint16_t value ) = 0;

		// DI
		virtual uint32_t getEDI() = 0;
		virtual void setEDI( uint32_t value ) = 0;
		virtual uint16_t getDI() = 0;
		virtual void setDI( uint16_t value ) = 0;

		// SP
		virtual uint32_t getESP() = 0;
		virtual void setESP( uint32_t value ) = 0;
		virtual uint16_t getSP() = 0;
		virtual void setSP( uint16_t value ) = 0;

		// BP
		virtual uint32_t getEBP() = 0;
		virtual void setEBP( uint32_t value ) = 0;
		virtual uint16_t getBP() = 0;
		virtual void setBP( uint16_t value ) = 0;

		// EFLAGS
		virtual uint32_t getEFLAGS() = 0;
		virtual void setEFLAGS( uint32_t value ) = 0;
		virtual bool getCF() = 0;
		virtual void setCF( bool value ) = 0;
		virtual bool getZF() = 0;
		virtual void setZF( bool value ) = 0;

		// EIP
		virtual uint32_t getEIP() = 0;
		virtual void setEIP( uint32_t value ) = 0;
		virtual void adjustEIP( int32_t offset ) = 0;

		// segment registers
		virtual uint16_t getSS() = 0;
		virtual void setSS( uint16_t value ) = 0;
		virtual uint16_t getCS() = 0;
		virtual void setCS( uint16_t value ) = 0;
		virtual uint16_t getDS() = 0;
		virtual void setDS( uint16_t value ) = 0;
		virtual uint16_t getES() = 0;
		virtual void setES( uint16_t value ) = 0;
		virtual uint16_t getFS() = 0;
		virtual void setFS( uint16_t value ) = 0;
		virtual uint16_t getGS() = 0;
		virtual void setGS( uint16_t value ) = 0;

		virtual uint32_t getReg32( int idx );
		virtual void setReg32( int idx, uint32_t value );
		virtual uint16_t getSegReg( int idx );
		virtual void setSegReg( int idx, uint16_t value );

		virtual void dump();
};


#endif  // __DOS32_CONTEXT_H__
