
#include "Debug.h"
#include "ExecutionEnvironment.h"
#include "Image.h"
#include "DescriptorTable.h"
#include "DOS.h"
#include "DOSExtender.h"


typedef int (*EntryPoint)(void);


DEFINE_INSTANCE( DOSExtender );


DOSExtender::DOSExtender( const DOS &dosServices ) :
	Singleton<DOSExtender>( this )
{
	ExecutionEnvironment &env = ExecutionEnvironment::getInstance();
	env.registerInterruptHandler( 0x21, int21Handler );

	uint16_t envSel;
	DescriptorTable &descTable = env.getDescriptorTable();
	PSP *psp = dosServices.getPsp();
	descTable.allocLdtDesc( (uint32_t) psp, 0x100, mPspSel );
	descTable.allocLdtDesc( (uint32_t) dosServices.getEnvironment(),
		dosServices.getEnvironmentSize(), envSel );
	psp->environmentSegment = envSel;
}

void DOSExtender::run( Image *img )
{
	mImage = img;
	EntryPoint p = (EntryPoint) img->getEntryPoint();
	void *stack = img->getStackPointer();
	TRACE( "running (%p) ...\n\n", p );	
	asm volatile ( "mov %%eax, %%es\n\t" \
	               "mov %1, %%esp\n\t" \
	               "jmp *%2\n\t" : : "a" (mPspSel), "r" (stack), "m" (p) );
}

bool DOSExtender::int21Handler( uint8_t idx, Context &ctx )
{
	/* Most DOS extenders hook INT 0x21 to override a number of functions, particularly
	   memory management functions and functions that take memory addresses as parameters.
	   All other function calls are reflected to the real mode interrupt handler. */

	bool canResume = true;

	uint8_t functionIdx = ctx.getAH();
	ctx.setCF( false );
	TRACE( "INT 0x21, function 0x%02x: ", functionIdx );

	switch ( functionIdx )
	{
		case 0x4A:
		{
			/* Resize a memory segment. There is no generally applicable way to grow a
			   segment, because the virtual address space beyond the (emulated) segment
			   limit might already be in use and most OSes don't provide a way to find
			   out. The image loader reserves a specified amount of memory at the end of
			   the data section, which most extenders will use as heap by resizing the
			   data segment. If the new limit is inside the reserved area, return success.
			 */
			// TODO: Linux has mremap and MEMREMAP_MAYMOVE

			uint16_t sel = ctx.getES();
			uint32_t newLimit = ctx.getEBX() * 16;
			TRACE( "resize segment 0x%02x, new limit = 0x%x\n", sel, newLimit );

			ExecutionEnvironment &env = ExecutionEnvironment::getInstance();
			DescriptorTable &descTable = env.getDescriptorTable();
			Descriptor *desc = descTable.getDesc( sel, true );
			if ( desc && ( desc->getSel() == descTable.getOsDataSel() ) )
			{
				if ( newLimit > (uint32_t) getInstance().mImage->getHeapEnd() )
				{
					TRACE( "heap space exhausted!\n" );
					ctx.setCF( true );
				}
			}
			else
				ctx.setCF( true );
			break;
		}
		case 0xED:
			TRACE( "OS/386 SegInfo (not implemented), BX = 0x%x (selector)\n",
				ctx.getBX() );
			ctx.setAL( 0 );  // if AL = 1, return "parent selector" in DI
			ctx.setCF( true );
			break;
		case 0xFF:
			TRACE( "DOS/4GW API" );
			canResume = handleDOS4GW( ctx );
			break;
		default:
			canResume = DOS::int21Handler( idx, ctx );
	}

	return canResume;
}

bool DOSExtender::handleDOS4GW( Context &ctx )
{
	uint8_t functionIdx = ctx.getDH();
	TRACE( ", function 0x%02x: ", functionIdx );

	switch ( functionIdx )
	{
		case 0x00:
			TRACE( "get version\n" );
			ctx.setEAX( 0x4734FFFF );  // high word == "4G"
			break;
		case 0x01 ... 0x17:
			TRACE( "not implemented\n" );
			ctx.setCF( true );
			break;
		default:
			TRACE( "invalid\n" );
			ctx.setCF( true );
	}

	return true;
}
