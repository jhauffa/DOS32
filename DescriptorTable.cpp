
#include "os/OS.h"
#include "Debug.h"
#include "DescriptorTable.h"


Descriptor::Descriptor( uint16_t sel ) :
	mType( DESC_TYPE_OS ), mBase( 0 ), mLimit( 0xFFFFFFFF ), mSel( sel ), mLDT( NULL )
{
}

Descriptor::Descriptor( host::LDT *ldt, uint32_t base, uint32_t limit ) :
	mType( DESC_TYPE_LDT ), mBase( base ), mLimit( limit ), mLDT( ldt )
{
	mSel = mLDT->allocDesc( base, limit );
}

Descriptor::~Descriptor()
{
	if ( mLDT )
		mLDT->freeDesc( mSel );
}

DescriptorType Descriptor::getType() const
{
	return mType;
}

uint32_t Descriptor::getLimit() const
{
	return mLimit;
}

uint32_t Descriptor::getBase() const
{
	return mBase;
}

uint16_t Descriptor::getSel() const
{
	return mSel;
}

bool Descriptor::setLimit( uint32_t limit )
{
	if ( mType == DESC_TYPE_LDT )
	{
		mLDT->setLimit( mSel, limit );
		return true;
	}
	return false;
}


DescriptorTable::DescriptorTable()
{
	mLDT = host::OS::createLDT();

	// get selectors provided by OS
	uint16_t sel;
	asm ( "mov %%cs, %%ax\n\t" : "=a" (sel) );
	mOSCodeSel = sel;
	allocOSDesc( sel );
	asm ( "mov %%ss, %%ax\n\t" : "=a" (sel) );
	allocOSDesc( sel );
	asm ( "mov %%ds, %%ax\n\t" : "=a" (sel) );
	mOSDataSel = sel;
	allocOSDesc( sel );
	asm ( "mov %%es, %%ax\n\t" : "=a" (sel) );
	allocOSDesc( sel );
	asm ( "mov %%fs, %%ax\n\t" : "=a" (sel) );
	allocOSDesc( sel );
	asm ( "mov %%gs, %%ax\n\t" : "=a" (sel) );
	allocOSDesc( sel );
}

DescriptorTable::~DescriptorTable()
{
	for ( std::map<uint16_t, Descriptor *>::const_iterator it = mDesc.begin();
	      it != mDesc.end(); ++it )
		delete it->second;
	delete mLDT;
}

Descriptor *DescriptorTable::getDesc( uint16_t sel )
{
	std::map<uint16_t, Descriptor *>::const_iterator pos = mDesc.find( sel );
	if ( pos != mDesc.end() )
		return pos->second;
	return NULL;
}

void DescriptorTable::allocOSDesc( uint16_t sel )
{
	Descriptor *desc = new Descriptor( sel );
	if ( !setDesc( sel, desc ) )
		delete desc;
}

void DescriptorTable::allocLDTDesc( uint32_t base, uint32_t limit, uint16_t &sel )
{
	Descriptor *desc = new Descriptor( mLDT, base, limit );
	sel = desc->getSel();
	if ( !setDesc( sel, desc ) )
		delete desc;
}

bool DescriptorTable::setDesc( uint16_t sel, Descriptor *desc )
{
	if ( sel == 0 )
		return false;

	if ( getDesc( sel ) != NULL )
	{
		if ( desc->getType() != DESC_TYPE_OS )
			ERR( "selector 0x%02x already exists\n", sel );
		return false;
	}

	mDesc[sel] = desc;
	return true;
}

uint16_t DescriptorTable::getOSCodeSel() const
{
	return mOSCodeSel;
}

uint16_t DescriptorTable::getOSDataSel() const
{
	return mOSDataSel;
}
