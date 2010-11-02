
#ifndef __DOS32_DOS_EXTENDER_H__
#define __DOS32_DOS_EXTENDER_H__

#include <stdint.h>

#include "os/Context.h"
#include "InterruptHandler.h"


class DOS;
class Image;
class ExecutionEnvironment;

class DOSExtender : public InterruptHandler
{
	public:
		DOSExtender( ExecutionEnvironment *env, DOS *dosServices );

		void run( Image *img );

		virtual bool handleInterrupt( uint8_t idx, Context &ctx );

	private:
		ExecutionEnvironment *mEnv;
		DOS *mDOS;
		Image *mImage;
		uint16_t mPspSel;

		bool handleDOS4GW( Context &ctx );
};


#endif  // __DOS32_DOS_EXTENDER_H__
