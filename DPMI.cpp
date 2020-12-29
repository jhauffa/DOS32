
#include <cstring>

#include "os/OS.h"
#include "os/OSException.h"
#include "Debug.h"
#include "ExecutionEnvironment.h"
#include "DPMI.h"


// RIVA.EXE cannot handle more than 256 MB
#define DPMI_MAX_MEMORY    0x0FFFF000
#define DPMI_MAX_PAGES   ( DPMI_MAX_MEMORY >> 12 )


DPMI::DPMI( ExecutionEnvironment *env ) : mEnv( env ), mAllocatedMemory( 0 )
{
	mEnv->registerInterruptHandler( 0x31, this );
}

DPMI::~DPMI()
{
	for ( std::map<uint32_t, host::MemMap *>::iterator it = mMemoryBlocks.begin();
	      it != mMemoryBlocks.end(); ++it )
		delete it->second;
}

uint32_t DPMI::allocateMemory( uint32_t size )
{
	host::MemMap *mem;
	try
	{
		mem = host::OS::createMemMap( size,
			host::MemMap::ACC_READ | host::MemMap::ACC_WRITE | host::MemMap::ACC_EXEC );
	}
	catch ( const host::OSException &ex )
	{
		return 0;
	}

	uint32_t addr = (uint32_t) mem->getPtr();
	mMemoryBlocks[addr] = mem;
	mAllocatedMemory += mem->getSize();
	return addr;
}

bool DPMI::handleInterrupt( uint8_t idx, host::Context &ctx )
{
	assert( idx == 0x31 );

	bool canResume = true;

	uint16_t functionIdx = ctx.getAX();
	ctx.setCF( false );
	TRACE( "INT 0x31, function 0x%04x: ", functionIdx );

	switch ( functionIdx )
	{
		case 0x0006:
		{
			uint16_t sel = ctx.getBX();
			TRACE( "get selector base, selector 0x%04x\n", sel );
			Descriptor *desc = mEnv->getDescriptorTable().getDesc( sel );
			if ( desc )
			{
				uint32_t base = desc->getBase();
				ctx.setCX( ( base >> 16 ) & 0xFFFF );
				ctx.setDX( base & 0xFFFF );
			}
			else
			{
				// invalid selector
				ctx.setAX( ERR_INVALID_SELECTOR );
				ctx.setCF( true );
			}
			break;
		}
		case 0x0500:
		{
			TRACE( "get free memory information\n" );
			Descriptor *desc = mEnv->getDescriptorTable().getDesc( ctx.getES() );
			if ( !desc )
				break;

			/* Most supported host systems have more RAM than most DOS apps can handle
			   (256 MB) and all of them can provide a virtually unlimited amount of memory
			   transparently by swapping to disk. Since most operating systems do not
			   provide a reliable method of determining the amount of unused physical
			   memory, we just report the maximum amount of memory as available. */
			uint32_t memUsed = mAllocatedMemory;
			DpmiMemoryInfo *info = (DpmiMemoryInfo *) ( desc->getBase() + ctx.getEDI() );
			memset( info, 0xFF, sizeof( DpmiMemoryInfo ) );
			info->largestFreeBlockSize = DPMI_MAX_MEMORY - memUsed;
			info->numPhysPages         = DPMI_MAX_PAGES;
			info->numFreePages         = DPMI_MAX_PAGES - ( memUsed >> 12 );
			info->maxUnlockedPageAlloc = info->numFreePages;
			info->numUnlockedPages     = info->numFreePages;
			break;
		}
		case 0x0501:
		{
			TRACE( "allocate memory block\n" );

			uint32_t size = ( ctx.getBX() << 16 ) | ctx.getCX();
			if ( size == 0 )
			{
				ctx.setAX( ERR_INVALID_VALUE );
				ctx.setCF( true );
				break;
			}

			uint32_t addr = allocateMemory( size );
			if ( addr == 0 )
			{
				ctx.setAX( ERR_NO_PHYS_MEM );
				ctx.setCF( true );
				break;
			}

			ctx.setBX( addr >> 16 );
			ctx.setCX( addr );
			ctx.setSI( addr >> 16 );
			ctx.setDI( addr );
			break;
		}
		default:
			FIXME( "not implemented\n" );
			canResume = false;
	}

	return canResume;
}
