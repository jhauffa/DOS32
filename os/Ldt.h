
#ifndef __DOS32_OS_LDT_H__
#define __DOS32_OS_LDT_H__

#include <cstdint>


class LDT
{
	public:
		virtual ~LDT() {}

		virtual uint16_t allocDesc( uint32_t base, uint32_t limit ) = 0;
		virtual void freeDesc( uint16_t sel ) = 0;
		virtual void setLimit( uint16_t sel, uint32_t limit ) = 0;
};


#endif  // __DOS32_OS_LDT_H__
