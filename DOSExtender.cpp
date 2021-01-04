
#include "os/Context.h"
#include "Debug.h"
#include "ExecutionEnvironment.h"
#include "Image.h"
#include "DescriptorTable.h"
#include "DOS.h"
#include "DOSExtender.h"


DOSExtender::DOSExtender( ExecutionEnvironment *env, DOS *dosServices ) :
	mEnv( env ), mDOS( dosServices )
{
	mEnv->registerInterruptHandler( 0x21, this );

	uint16_t envSel;
	DescriptorTable &descTable = mEnv->getDescriptorTable();
	PSP *psp = mDOS->getPsp();
	descTable.allocLDTDesc( (uint32_t) psp, 0x100, mPspSel );
	descTable.allocLDTDesc( (uint32_t) mDOS->getEnvironment(), mDOS->getEnvironmentSize(),
		envSel );
	psp->environmentSegment = envSel;
}

void DOSExtender::run( Image *img )
{
	mImage = img;
	void *entry = img->getEntryPoint();
	void *stack = img->getStackPointer();
	TRACE( "running (%p) ...\n\n", entry );	
	asm volatile ( "mov %%eax, %%es\n\t" \
	               "mov %1, %%esp\n\t" \
	               "jmp *%2\n\t" : : "a" (mPspSel), "r" (stack), "m" (entry) );
}

bool DOSExtender::handleInterrupt( uint8_t idx, host::Context &ctx )
{
	/* Most DOS extenders hook INT 0x21 to override a number of functions, particularly
	   memory management functions and functions that take memory addresses as parameters.
	   All other function calls are reflected to the real mode interrupt handler. */
	assert( idx == 0x21 );

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
			   data segment. If the new limit is inside the reserved area, return
			   success. */
			uint16_t sel = ctx.getES();
			uint32_t newLimit = ctx.getEBX() * 16;
			TRACE( "resize segment 0x%02x, new limit = 0x%x\n", sel, newLimit );

			DescriptorTable &descTable = mEnv->getDescriptorTable();
			Descriptor *desc = descTable.getDesc( sel );
			if ( desc && ( desc->getSel() == descTable.getOSDataSel() ) )
			{
				if ( newLimit > (uint32_t) mImage->getHeapEnd() )
				{
					ERR( "heap space exhausted!\n" );
					ctx.setCF( true );
				}
			}
			else
				ctx.setCF( true );
			// TODO: should set EAX and EBX in case of error
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

		// all other functions are forwarded to DOS with address translation as required
		case 0x1A:
			TRACE( "set disk transfer address\n" );
			mDOS->setDTA( (char *) ctx.getEDX() );
			break;
		case 0x3B:
			TRACE( "set current directory\n" );
			mDOS->setCurrentDirectory( (const char *) ctx.getEDX(), ctx );
			break;
		case 0x3D:
			TRACE( "open\n" );
			mDOS->fileOpen( (char *) ctx.getEDX(), ctx );
			break;
		case 0x3E:
			TRACE( "close\n" );
			mDOS->fileClose( ctx );
			break;
		case 0x3F:
			TRACE( "read\n" );
			mDOS->fileRead( (char *) ctx.getEDX(), ctx );
			break;
		case 0x40:
			TRACE( "write\n" );
			mDOS->fileWrite( (const char *) ctx.getEDX(), ctx );
			break;
		case 0x47:
			TRACE( "get current directory\n" );
			mDOS->getCurrentDirectory( (char *) ctx.getESI(), ctx );
			break;
		default:
			canResume = mDOS->handleInterrupt( idx, ctx, NULL );
	}

	return canResume;
}

bool DOSExtender::handleDOS4GW( host::Context &ctx )
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
			FIXME( "not implemented\n" );
			ctx.setCF( true );
			break;
		default:
			ERR( "invalid function call\n" );
			ctx.setCF( true );
	}

	return true;
}
