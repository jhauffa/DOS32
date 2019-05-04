
#ifndef __DOS32_EXECUTION_ENVIRONMENT_H__
#define __DOS32_EXECUTION_ENVIRONMENT_H__

#include <stdint.h>
#include <map>

#include "Singleton.h"
#include "DescriptorTable.h"


class Image;
class Context;
class ExceptionInfo;
class DOS;
class DOSExtender;
class DPMI;
class InterruptHandler;


class ExecutionEnvironment : public Singleton<ExecutionEnvironment>
{
	public:
		ExecutionEnvironment( int argc, char *argv[], char *envp[] );
		~ExecutionEnvironment();

		void registerInterruptHandler( uint8_t idx, InterruptHandler *handler );

		int run( Image *img );

		DescriptorTable &getDescriptorTable();

	private:
		std::map<uint8_t, InterruptHandler *> mInterruptHandlers;
		DescriptorTable mDescriptorTable;

		DOS *mDOS;
		DOSExtender *mDOSExtender;
		DPMI *mDPMI;

		static int appThreadProc( void *data );
		static void memoryExceptionHandler( ExceptionInfo &info );
		static void consoleInterruptHandler( ExceptionInfo &info );

		static bool handleSegmentRegisterAccess( uint8_t *data, Context &ctx,
			bool addrSizeOverride, int &instrSize );
		static bool loadSelectorAlias( Context &ctx, int segmentReg, uint16_t selector );

		static int decodePrefix( uint8_t *data, int &segmentOverride,
			bool &dataSizeOverride, bool &addrSizeOverride );
		static int decodeModRm( uint8_t *data, Context &ctx, bool addrSizeOverride,
			int &regOp1, int &regOp2, uint32_t &memOp, bool &hasMemOp );
};

DEFINE_INSTANCE( ExecutionEnvironment );


#endif  // __DOS32_EXECUTION_ENVIRONMENT_H__
