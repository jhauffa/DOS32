
#ifndef __DOS32_EXECUTION_ENVIRONMENT_H__
#define __DOS32_EXECUTION_ENVIRONMENT_H__

#include <cstdint>
#include <map>

#include "Singleton.h"
#include "DescriptorTable.h"


namespace host {
class ExceptionInfo;
}

class Image;
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
		static void memoryExceptionHandler( host::ExceptionInfo &info );
		static void consoleInterruptHandler( host::ExceptionInfo &info );

		static int decodePrefix( uint8_t *data );
};

DEFINE_INSTANCE( ExecutionEnvironment );


#endif  // __DOS32_EXECUTION_ENVIRONMENT_H__
