
#ifndef __DOS32_OS_CONTEXT_H__
#define __DOS32_OS_CONTEXT_H__

#include <cstdint>


class Context
{
	public:
		virtual ~Context() {}

		// AX
		virtual uint32_t getEAX() const = 0;
		virtual void setEAX( uint32_t value ) = 0;
		virtual uint16_t getAX() const = 0;
		virtual void setAX( uint16_t value ) = 0;
		virtual uint8_t getAH() const = 0;
		virtual void setAH( uint8_t value ) = 0;
		virtual uint8_t getAL() const = 0;
		virtual void setAL( uint8_t value ) = 0;

		// BX
		virtual uint32_t getEBX() const = 0;
		virtual void setEBX( uint32_t value ) = 0;
		virtual uint16_t getBX() const = 0;
		virtual void setBX( uint16_t value ) = 0;
		virtual uint8_t getBH() const = 0;
		virtual void setBH( uint8_t value ) = 0;
		virtual uint8_t getBL() const = 0;
		virtual void setBL( uint8_t value ) = 0;

		// CX
		virtual uint32_t getECX() const = 0;
		virtual void setECX( uint32_t value ) = 0;
		virtual uint16_t getCX() const = 0;
		virtual void setCX( uint16_t value ) = 0;
		virtual uint8_t getCH() const = 0;
		virtual void setCH( uint8_t value ) = 0;
		virtual uint8_t getCL() const = 0;
		virtual void setCL( uint8_t value ) = 0;

		// DX
		virtual uint32_t getEDX() const = 0;
		virtual void setEDX( uint32_t value ) = 0;
		virtual uint16_t getDX() const = 0;
		virtual void setDX( uint16_t value ) = 0;
		virtual uint8_t getDH() const = 0;
		virtual void setDH( uint8_t value ) = 0;
		virtual uint8_t getDL() const = 0;
		virtual void setDL( uint8_t value ) = 0;

		// SI
		virtual uint32_t getESI() const = 0;
		virtual void setESI( uint32_t value ) = 0;
		virtual uint16_t getSI() const = 0;
		virtual void setSI( uint16_t value ) = 0;

		// DI
		virtual uint32_t getEDI() const = 0;
		virtual void setEDI( uint32_t value ) = 0;
		virtual uint16_t getDI() const = 0;
		virtual void setDI( uint16_t value ) = 0;

		// SP
		virtual uint32_t getESP() const = 0;
		virtual void setESP( uint32_t value ) = 0;
		virtual uint16_t getSP() const = 0;
		virtual void setSP( uint16_t value ) = 0;

		// BP
		virtual uint32_t getEBP() const = 0;
		virtual void setEBP( uint32_t value ) = 0;
		virtual uint16_t getBP() const = 0;
		virtual void setBP( uint16_t value ) = 0;

		// EFLAGS
		virtual uint32_t getEFLAGS() const = 0;
		virtual void setEFLAGS( uint32_t value ) = 0;
		virtual bool getCF() const = 0;
		virtual void setCF( bool value ) = 0;
		virtual bool getZF() const = 0;
		virtual void setZF( bool value ) = 0;

		// EIP
		virtual uint32_t getEIP() const = 0;
		virtual void setEIP( uint32_t value ) = 0;
		virtual void adjustEIP( int32_t offset ) = 0;

		// segment registers
		virtual uint16_t getSS() const = 0;
		virtual void setSS( uint16_t value ) = 0;
		virtual uint16_t getCS() const = 0;
		virtual void setCS( uint16_t value ) = 0;
		virtual uint16_t getDS() const = 0;
		virtual void setDS( uint16_t value ) = 0;
		virtual uint16_t getES() const = 0;
		virtual void setES( uint16_t value ) = 0;
		virtual uint16_t getFS() const = 0;
		virtual void setFS( uint16_t value ) = 0;
		virtual uint16_t getGS() const = 0;
		virtual void setGS( uint16_t value ) = 0;

		virtual uint32_t getReg32( int idx ) const;
		virtual void setReg32( int idx, uint32_t value );
		virtual uint16_t getSegReg( int idx ) const;
		virtual void setSegReg( int idx, uint16_t value );

		virtual void dump() const;
};


#endif  // __DOS32_CONTEXT_H__
