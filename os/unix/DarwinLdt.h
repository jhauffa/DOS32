
#ifndef __DOS32_OS_UNIX_DARWIN_LDT_H__
#define __DOS32_OS_UNIX_DARWIN_LDT_H__

#include "os/Ldt.h"


class DarwinLdt : public Ldt
{
	public:
		virtual ~DarwinLdt() {}

		virtual uint16_t allocDesc( uint32_t base, uint32_t limit );
		virtual void freeDesc( uint16_t sel );
		virtual void setLimit( uint16_t sel, uint32_t limit );

	private:
		static void setDescLimit( void *desc, uint32_t limit );
};


#endif  // __DOS32_OS_UNIX_DARWIN_LDT_H__
