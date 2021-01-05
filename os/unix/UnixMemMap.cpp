
#include <sys/mman.h>
#include <unistd.h>
#include <cerrno>

#include "Debug.h"
#include "os/File.h"
#include "os/unix/UnixException.h"
#include "os/unix/UnixMemMap.h"


namespace host {

UnixMemMap::UnixMemMap( const File &file, int access )
{
	mSize = file.getSize();
	mProtFlags = convertFlags( access );
	mPtr = mmap( NULL, mSize, mProtFlags, MAP_PRIVATE, file.getHandle(), 0 );
	if ( mPtr == MAP_FAILED )
		throw UnixException();
	mEnd = (uint8_t *) mPtr + mSize;
}

UnixMemMap::UnixMemMap( MemSize size, int access ) : mSize( size )
{
	mProtFlags = convertFlags( access );
	mPtr = mmap( NULL, mSize, mProtFlags, MAP_ANON | MAP_PRIVATE, 0, 0 );
	if ( mPtr == MAP_FAILED )
		throw UnixException();
	mEnd = (uint8_t *) mPtr + mSize;
}

UnixMemMap::~UnixMemMap()
{
	for (std::list<SubMapping>::iterator it = mSubMappings.begin();
	     it != mSubMappings.end(); ++it)
		munmap( it->addr, it->length );
	munmap( mPtr, mSize );
}

void UnixMemMap::map( const File &file, MemSize regionOffset, MemSize fileOffset,
	MemSize length )
{
	assert ( isInRange( regionOffset ) && isInRange( regionOffset + length - 1 ) );

	void *addr = (uint8_t *) mPtr + regionOffset;

	if ( ( ( (uintptr_t) addr & 0xFFF ) == 0 ) && ( ( fileOffset & 0xFFF ) == 0 ) )
	{
		void *ptr = mmap( addr, length, mProtFlags, MAP_FIXED | MAP_PRIVATE,
			file.getHandle(), fileOffset );
		if ( ptr == (void *) -1 )
			throw UnixException();

		SubMapping m;
		m.addr = ptr;
		m.length = length;
		mSubMappings.push_back( m );
	}
	else
	{
		// target address or file offset not page aligned, read directly
		int fd = dup( file.getHandle() );
		lseek( fd, fileOffset, SEEK_SET );
		mprotect( addr, length, PROT_READ | PROT_WRITE );
		ssize_t bytesRead = read( fd, addr, length );
		mprotect( addr, length, mProtFlags );
		if ( bytesRead == -1 )
		{
			int error = errno;
			close( fd );
			throw UnixException( error );
		}
		close( fd );
	}
}

void *UnixMemMap::getPtr() const
{
	return mPtr;
}

MemSize UnixMemMap::getSize() const
{
	return mSize;
}

bool UnixMemMap::isInRange( void *ptr ) const
{
	return ( ptr < mEnd );
}

bool UnixMemMap::isInRange( MemSize offset ) const
{
	return ( offset < mSize );
}

int UnixMemMap::convertFlags( int access )
{
	int prot = 0;
	if ( access & ACC_READ )
		prot |= PROT_READ;
	if ( access & ACC_WRITE )
		prot |= PROT_WRITE;
	if ( access & ACC_EXEC )
		prot |= PROT_EXEC;
	if ( prot == 0 )
		prot = PROT_NONE;
	return prot;
}

}
