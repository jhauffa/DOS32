
#ifndef __DOS32_OS_MEMMAP_H__
#define __DOS32_OS_MEMMAP_H__

#include <sys/types.h>

#include "os/File.h"

typedef off_t MemSize;


class MemMap
{
	public:
		enum Access { ACC_READ = 1, ACC_WRITE = 2, ACC_EXEC = 4 };

		virtual ~MemMap() {}

		virtual void map( const File &file, MemSize regionOffset, FileSize fileOffset,
			MemSize length ) = 0;

		virtual void *getPtr() const = 0;
		virtual MemSize getSize() const = 0;

		virtual bool isInRange( void *ptr ) const = 0;
		virtual bool isInRange( MemSize offset ) const = 0;
};


#endif  // __DOS32_OS_MEMMAP_H__
