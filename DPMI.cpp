
#include <string.h>

#include "os/OS.h"
#include "os/OsException.h"
#include "Debug.h"
#include "ExecutionEnvironment.h"
#include "DPMI.h"


// RIVA.EXE cannot handle more than 256 MB
#define DPMI_MAX_MEMORY    0x0FFFF000
#define DPMI_MAX_PAGES   ( DPMI_MAX_MEMORY >> 12 )


DEFINE_INSTANCE( DPMI );


DPMI::DPMI() :
	Singleton<DPMI>( this ), mAllocatedMemory( 0 )
{
	ExecutionEnvironment::getInstance().registerInterruptHandler( 0x31, int31Handler );
}

DPMI::~DPMI()
{
	for ( std::map<uint32_t, MemMap *>::iterator it = mMemoryBlocks.begin();
	      it != mMemoryBlocks.end(); ++it )
		delete it->second;
}

uint32_t DPMI::allocateMemory( uint32_t size )
{
	MemMap *mem;
	try
	{
		mem = OS::createMemMap( size,
			MemMap::ACC_READ | MemMap::ACC_WRITE | MemMap::ACC_EXEC );
	}
	catch ( const OsException &ex )
	{
		return 0;
	}

	uint32_t addr = (uint32_t) mem->getPtr();
	mMemoryBlocks[addr] = mem;
	mAllocatedMemory += mem->getSize();
	return addr;
}

bool DPMI::int31Handler( uint8_t idx, Context &ctx )
{
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
			Descriptor *desc = ExecutionEnvironment::getInstance().
				getDescriptorTable().getDesc( sel, true );
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
			Descriptor *desc = ExecutionEnvironment::getInstance().
				getDescriptorTable().getDesc( ctx.getES(), true );
			if ( !desc )
				break;

			/* Most supported host systems have more RAM than most DOS apps can handle
			   (256 MB) and all of them can provide a virtually unlimited amount of memory
			   transparently by swapping to disk. Since most operating systems do not
			   provide a reliable method of determining the amount of unused physical
			   memory, we just report the maximum amount of memory as available.
			 */
			// TODO: Darwin has RLIMIT_RSS, RLIMIT_MEMLOCK, CTL_HW/HW_MEMSIZE/HW_USERMEM
			uint32_t memUsed = getInstance().mAllocatedMemory;
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

			uint32_t addr = getInstance().allocateMemory( size );
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
			TRACE( "not implemented\n" );
			canResume = false;
	}

	return canResume;
}