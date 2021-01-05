
#ifndef __DOS32_INTERRUPT_HANDLER_H__
#define __DOS32_INTERRUPT_HANDLER_H__

#include <cstdint>

namespace host {
class Context;
}


class InterruptHandler
{
	public:
		// TODO: add a separate handler for interrupts raised in a 16-bit context
		virtual bool handleInterrupt( uint8_t idx, host::Context &ctx ) = 0;
};


#endif  // __DOS32_INTERRUPT_HANDLER_H__
