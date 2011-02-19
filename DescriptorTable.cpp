
#include "os/OS.h"
#include "Debug.h"
#include "DescriptorTable.h"


Descriptor::Descriptor( uint16_t sel ) :
	mType( DESC_TYPE_OS ), mBase( 0 ), mLimit( 0xFFFFFFFF ), mSel( sel ), mAliasSel( 0 ),
	mLdt( NULL )
{
}

Descriptor::Descriptor( Ldt *ldt, uint32_t base, uint32_t limit ) :
	mType( DESC_TYPE_LDT ), mBase( base ), mLimit( limit ), mAliasSel( 0 ), mLdt( ldt )
{
	mSel = mLdt->allocDesc( base, limit );
}

Descriptor::Descriptor( uint16_t sel, uint16_t aliasSel ) :
	mType( DESC_TYPE_ALIAS ), mBase( 0 ), mLimit( 0 ), mSel( sel ), mAliasSel( aliasSel ),
	mLdt( NULL )
{
}

Descriptor::~Descriptor()
{
	if ( mLdt )
		mLdt->freeDesc( mSel );
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

uint16_t Descriptor::getAliasSel() const
{
	return mAliasSel;
}

bool Descriptor::setLimit( uint32_t limit )
{
	if ( mType == DESC_TYPE_LDT )
	{
		mLdt->setLimit( mSel, limit );
		return true;
	}
	return false;
}

bool Descriptor::setAliasSel( uint16_t aliasSel )
{
	if ( mType != DESC_TYPE_LDT )
	{
		mAliasSel = aliasSel;
		return true;
	}
	return false;
}


DescriptorTable::DescriptorTable()
{
	mLdt = OS::createLdt();

	// get selectors provided by OS
	uint16_t sel;
	asm ( "mov %%cs, %%ax\n\t" : "=a" (sel) );
	mOsCodeSel = sel;
	allocOsDesc( sel );
	asm ( "mov %%ss, %%ax\n\t" : "=a" (sel) );
	allocOsDesc( sel );
	asm ( "mov %%ds, %%ax\n\t" : "=a" (sel) );
	mOsDataSel = sel;
	allocOsDesc( sel );
	asm ( "mov %%es, %%ax\n\t" : "=a" (sel) );
	allocOsDesc( sel );
	asm ( "mov %%fs, %%ax\n\t" : "=a" (sel) );
	allocOsDesc( sel );
	asm ( "mov %%gs, %%ax\n\t" : "=a" (sel) );
	allocOsDesc( sel );
}

DescriptorTable::~DescriptorTable()
{
	for ( std::map<uint16_t, Descriptor *>::const_iterator it = mDesc.begin();
	      it != mDesc.end(); ++it )
		delete it->second;
	delete mLdt;
}

Descriptor *DescriptorTable::getDesc( uint16_t sel, bool resolveAlias )
{
	Descriptor *desc = NULL;
	std::map<uint16_t, Descriptor *>::const_iterator pos = mDesc.find( sel );
	if ( pos != mDesc.end() )
		desc = pos->second;
	if ( resolveAlias && ( desc && ( desc->getType() == DESC_TYPE_ALIAS ) ) )
		desc = getDesc( desc->getAliasSel(), false );
	return desc;
}

void DescriptorTable::allocOsDesc( uint16_t sel )
{
	Descriptor *desc = new Descriptor( sel );
	if ( !setDesc( sel, desc ) )
		delete desc;
}

void DescriptorTable::allocLdtDesc( uint32_t base, uint32_t limit, uint16_t &sel )
{
	Descriptor *desc = new Descriptor( mLdt, base, limit );
	sel = desc->getSel();
	if ( !setDesc( sel, desc ) )
		delete desc;
}

void DescriptorTable::allocAliasDesc( uint16_t sel, uint16_t aliasSel )
{
	Descriptor *desc = getDesc( sel, false );
	if ( !desc )
	{
		desc = new Descriptor( sel, aliasSel );
		if ( !setDesc( sel, desc ) )
			delete desc;
	}
	else
		desc->setAliasSel( aliasSel );
}

bool DescriptorTable::setDesc( uint16_t sel, Descriptor *desc )
{
	if ( sel == 0 )
		return false;

	if ( getDesc( sel, false ) != NULL )
	{
		if ( desc->getType() != DESC_TYPE_OS )
			ERR( "selector 0x%02x already exists\n", sel );
		return false;
	}

	mDesc[sel] = desc;
	return true;
}

uint16_t DescriptorTable::getOsCodeSel() const
{
	return mOsCodeSel;
}

uint16_t DescriptorTable::getOsDataSel() const
{
	return mOsDataSel;
}
