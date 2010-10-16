
#ifndef __DOS32_DOS_EXTENDER_H__
#define __DOS32_DOS_EXTENDER_H__

#include <stdint.h>

#include "os/Context.h"
#include "Singleton.h"


class DOS;
class Image;

class DOSExtender : public Singleton<DOSExtender>
{
	public:
		DOSExtender( const DOS &dosServices );

		void run( Image *img );

	private:
		Image *mImage;
		uint16_t mPspSel;

		static bool int21Handler( uint8_t idx, Context &ctx );
		static bool handleDOS4GW( Context &ctx );
};


#endif  // __DOS32_DOS_EXTENDER_H__
