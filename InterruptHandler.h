
#ifndef __DOS32_INTERRUPT_HANDLER_H__
#define __DOS32_INTERRUPT_HANDLER_H__

#include <cstdint>

class Context;


class InterruptHandler
{
	public:
		virtual bool handleInterrupt( uint8_t idx, Context &ctx ) = 0;
};


#endif  // __DOS32_INTERRUPT_HANDLER_H__
