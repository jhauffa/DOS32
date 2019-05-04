
#include <architecture/i386/table.h>
#include <i386/user_ldt.h>
#include <string.h>

#include "os/unix/UnixException.h"
#include "os/unix/DarwinLdt.h"


uint16_t DarwinLdt::allocDesc( uint32_t base, uint32_t limit )
{
	data_desc_t desc;
	// anonymous members have to be 0!
	memset( &desc, 0, sizeof(data_desc_t) );
	setDescLimit( &desc, limit );
	desc.base00 = base & 0xFFFF;
	desc.base16 = ( base >> 16 ) & 0xFF;
	desc.base24 = ( base >> 24 ) & 0xFF;
	desc.type = DESC_DATA_WRITE;
	desc.dpl = 3;
	desc.present = 1;
	desc.stksz = DESC_DATA_32B;

	int sel = i386_set_ldt( LDT_AUTO_ALLOC, (ldt_entry *) &desc, 1 );
	if ( sel == -1 )
		throw UnixException();
	sel = ( sel << 3 ) | 0x07;
	return sel;
}

void DarwinLdt::freeDesc( uint16_t sel )
{
	if ( i386_set_ldt( sel, NULL, 1 ) == -1 )
		throw UnixException();
}

void DarwinLdt::setLimit( uint16_t sel, uint32_t limit )
{
	data_desc_t desc;
	int selIdx = sel >> 3;
	if ( i386_get_ldt( selIdx, (ldt_entry *) &desc, 1 ) == -1 )
		throw UnixException();
	setDescLimit( &desc, limit );
	if ( i386_set_ldt( selIdx, (ldt_entry *) &desc, 1 ) == -1 )
		throw UnixException();	
}

void DarwinLdt::setDescLimit( void *desc, uint32_t limit )
{
	data_desc_t *d = (data_desc_t *) desc;
	if ( limit >= 0x100000 )
	{
		limit >>= 12;
		d->granular = 1;
	}
	else
		d->granular = 0;
	d->limit00 = limit & 0xFFFF;
	d->limit16 = ( limit >> 16 ) & 0x0F;
}
