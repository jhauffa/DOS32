
#ifndef __DOS32_OS_UNIX_MEMMAP_H__
#define __DOS32_OS_UNIX_MEMMAP_H__

#include <list>
#include <sys/types.h>

#include "os/MemMap.h"


class UnixMemMap : public MemMap
{
	public:
		UnixMemMap( const std::string &fileName, int access );
		UnixMemMap( MemSize size, int access );
		virtual ~UnixMemMap();

		virtual void map( const MemMap &fileMap, MemSize regionOffset, MemSize fileOffset,
			MemSize length );

		virtual void *getPtr() const;
		virtual MemSize getSize() const;

		virtual bool isInRange( void *ptr ) const;
		virtual bool isInRange( MemSize offset ) const;

	protected:
		virtual void *getFileHandle() const;

	private:
		int mFd;
		off_t mSize;
		void *mPtr;
		void *mEnd;
		int mProtFlags;

		struct SubMapping
		{
			void *addr;
			off_t length;
		};
		std::list<SubMapping> mSubMappings;

		static int convertFlags( int access );
};


#endif  // __DOS32_OS_UNIX_MEMMAP_H__
