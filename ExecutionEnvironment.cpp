
#include <stdlib.h>

#include "os/OS.h"
#include "os/ExceptionInfo.h"
#include "Debug.h"
#include "Registers.h"
#include "DOSExtender.h"
#include "ExecutionEnvironment.h"


DEFINE_INSTANCE( ExecutionEnvironment );


ExecutionEnvironment::ExecutionEnvironment() :
	Singleton<ExecutionEnvironment>( this )
{
}

void ExecutionEnvironment::registerInterruptHandler( uint8_t idx,
	InterruptHandler handler )
{
	mInterruptHandlers[idx] = handler;
}

DescriptorTable &ExecutionEnvironment::getDescriptorTable()
{
	return mDescriptorTable;
}

int ExecutionEnvironment::run( Image *img )
{
	Thread *thr = OS::createThread( appThreadProc, img );
	thr->run();
	int result = thr->join();
	delete thr;
	return result;
}

int ExecutionEnvironment::appThreadProc( void *data )
{
	ExceptionManager &mgr = OS::getExceptionManager();
	mgr.setMemoryExceptionHandler( memoryExceptionHandler );
	mgr.setConsoleInterruptHandler( consoleInterruptHandler );

	// run application
	DOSExtender::getInstance().run( (Image *) data );
	return 0;  // never happens
}

void ExecutionEnvironment::memoryExceptionHandler( ExceptionInfo &info )
{
	Context &ctx = info.getContext();
	uint8_t *eip = (uint8_t *) ctx.getEIP();
	bool canResume = true;

	// parse instruction prefix
	int segmentOverride;
	bool dataSizeOverride;
	bool addrSizeOverride;
	int instrSize = decodePrefix( eip, segmentOverride, dataSizeOverride,
		addrSizeOverride );
	eip += instrSize;

	// parse instruction
	switch ( eip[0] )
	{
		case 0x8E:
			// move to segment register
			instrSize += 1;
			canResume = handleSegmentRegisterAccess( &eip[1], ctx, addrSizeOverride,
				instrSize );
			break;
		case 0xCD:
		{
			// interrupt
			InterruptHandler hdl = getInstance().mInterruptHandlers[eip[1]];
			if ( !hdl )
			{
				TRACE( "TODO: interrupt 0x%02x, AX = 0x%04x\n", eip[1], ctx.getAX() );
				canResume = false;
			}
			else
				canResume = hdl( eip[1], ctx );
			instrSize += 2;
			break;
		}
		case 0xFA:
			TRACE( "clear interrupt flag\n" );
			instrSize += 1;
			break;
		case 0xFB:
			TRACE( "set interrupt flag\n" );
			instrSize += 1;
			break;
		default:
		{
			// handle aliasing of valid selectors that cannot be used for memory access
			int segmentReg = segmentOverride;
			if ( segmentReg == SEG_NONE )
				segmentReg = SEG_DS;
			uint16_t sel = ctx.getSegReg( segmentReg );

			canResume = loadSelectorAlias( ctx, segmentReg, sel );
			instrSize = 0;  // restart instruction
			break;
		}
	}

	if ( !canResume )
	{
		info.dump();
		exit( 2 );
	}
	else
	{
		ctx.adjustEIP( instrSize );
//		TRACE( "continuing\n" );
	}
}

void ExecutionEnvironment::consoleInterruptHandler( ExceptionInfo &info )
{
	Context &ctx = info.getContext();
	TRACE( "\nlast EIP = 0x%x\n", ctx.getEIP() );
	exit( 3 );
}

bool ExecutionEnvironment::handleSegmentRegisterAccess( uint8_t *data, Context &ctx,
	bool addrSizeOverride, int &instrSize )
{
	int segmentReg, selectorReg;
	uint32_t selectorAddr;
	bool hasMemOp;
	instrSize += decodeModRm( data, ctx, addrSizeOverride, segmentReg, selectorReg,
		selectorAddr, hasMemOp );
	assert( segmentReg < 6 );

	uint16_t selector;
	if ( hasMemOp )
		selector = *( (uint32_t *) selectorAddr ) & 0xFFFF;
	else
		selector = ctx.getReg32( selectorReg ) & 0xFFFF;

	return loadSelectorAlias( ctx, segmentReg, selector );
}

bool ExecutionEnvironment::loadSelectorAlias( Context &ctx, int segmentReg,
	uint16_t selector )
{
	Descriptor *desc = getInstance().mDescriptorTable.getDesc( selector, false );
	if ( desc && ( desc->getAliasSel() != 0 ) )
	{
		TRACE( "segment register %d: selector 0x%02x -> 0x%02x\n", segmentReg, selector,
			desc->getAliasSel() );
		ctx.setSegReg( segmentReg, desc->getAliasSel() );
		return true;
	}

	TRACE( "trying to load invalid selector 0x%02x\n", selector );
	return false;
}

int ExecutionEnvironment::decodePrefix( uint8_t *data, int &segmentOverride,
	bool &dataSizeOverride, bool &addrSizeOverride )
{
	segmentOverride = SEG_NONE;
	dataSizeOverride = false;
	addrSizeOverride = false;

	int instrStart = 0;
	bool foundInstr = false;
	while ( !foundInstr )
	{
		switch ( data[instrStart] )
		{
			case 0x66:
				dataSizeOverride = true;
				break;
			case 0x67:
				addrSizeOverride = true;
				break;
			case 0x26:
				segmentOverride = SEG_ES;
				break;
			case 0x2E:
				segmentOverride = SEG_CS;
				break;
			case 0x36:
				segmentOverride = SEG_SS;
				break;
			case 0x3E:
				segmentOverride = SEG_DS;
				break;
			case 0x64:
				segmentOverride = SEG_FS;
				break;
			case 0x65:
				segmentOverride = SEG_GS;
				break;
			case 0xF0:  // LOCK
			case 0xF2:  // REPNE
			case 0xF3:	// REP
				break;
			default:
				foundInstr = true;
				break;
		}
		if ( !foundInstr )
			instrStart++;
	}

	return instrStart;
}

int ExecutionEnvironment::decodeModRm( uint8_t *data, Context &ctx, bool addrSizeOverride,
	int &regOp1, int &regOp2, uint32_t &memOp, bool &hasMemOp )
{
	if ( addrSizeOverride )
		TRACE( "TODO: address size override\n" );

	int instrSize = 1;
	bool hasSib = false;  // SIB: 0-2 base, 3-5 index, 6-7 scale
	bool hasDisp8 = false;
	bool hasDisp32 = false;

	int scale = 1;
	uint32_t index = 0;
	uint32_t base = 0;

	uint8_t rm = data[0] & 0x07;  // ModR/M bit 0-2: R/M
	regOp1 = ( data[0] >> 3 ) & 0x07;  // ModR/M bit 3-5: REG
	uint8_t mod = ( data[0] >> 6 ) & 0x03;  // ModR/M bit 6-7: Mod

	if ( mod == 0x03 )
	{
		// second operand is a register
		regOp2 = rm;
		hasMemOp = false;
	}
	else
	{
		// second operand is a (register indirect) memory address
		if ( rm == 0x04 )
			hasSib = true;
		else if ( rm == 0x05 )
		{
			if ( mod == 0x00 )
				hasDisp32 = true;
		}
		else
			index = ctx.getReg32( rm );

		if ( mod == 0x01 )
			hasDisp8 = true;
		else if ( mod == 0x02 )
			hasDisp32 = true;

		hasMemOp = true;
	}

	if ( hasSib )
	{
		uint8_t bv = data[1] & 0x07;  // SIB bit 0-2: base
		uint8_t iv = ( data[1] >> 3 ) & 0x07;  // SIB bit 3-5: index
		uint8_t sv = ( data[1] >> 6 ) & 0x03;  // SIB bit 6-7: scale

		scale = 1 << sv;
		if ( iv != 0x04 )
			index = ctx.getReg32( iv );
		if ( !( ( mod == 0x00 ) && ( bv == 0x05 ) ) )
			base = ctx.getReg32( bv );

		instrSize++;
	}

	if ( hasDisp8 )
	{
		int8_t disp = (int8_t) data[instrSize];
		index += disp;
		instrSize += 1;
	}
	else if ( hasDisp32 )
	{
		uint32_t disp = *( (uint32_t *) &data[instrSize] );
		index += disp;
		instrSize += 4;
	}

	if ( hasMemOp )
		memOp = ( scale * index ) + base;

	return instrSize;
}
