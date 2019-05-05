
#ifndef __DOS32_DPMI_H__
#define __DOS32_DPMI_H__

#include <map>
#include <cstdint>

#include "InterruptHandler.h"


struct DpmiMemoryInfo
{
	uint32_t largestFreeBlockSize;
	uint32_t maxUnlockedPageAlloc;
	uint32_t maxLockedPageAlloc;
	uint32_t addrSpaceSizePages;
	uint32_t numUnlockedPages;
	uint32_t numFreePages;
	uint32_t numPhysPages;
	uint32_t addrSpaceFreePages;
	uint32_t pageFileSizePages;
	uint8_t reserved[12];
} __attribute__ ((packed));


class Context;
class MemMap;
class ExecutionEnvironment;

class DPMI : public InterruptHandler
{
	public:
		DPMI( ExecutionEnvironment *env );
		~DPMI();

		virtual bool handleInterrupt( uint8_t idx, Context &ctx );

	private:
		enum Error {
			ERR_NO_PHYS_MEM = 0x8013,
			ERR_INVALID_VALUE = 0x8021,
			ERR_INVALID_SELECTOR = 0x8022
		};

		ExecutionEnvironment *mEnv;
		std::map<uint32_t, MemMap *> mMemoryBlocks;
		uint32_t mAllocatedMemory;

		uint32_t allocateMemory( uint32_t size );
};


#endif  // __DOS32_DPMI_H__
