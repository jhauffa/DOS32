
#ifndef __DOS32_OS_FILE_H__
#define __DOS32_OS_FILE_H__

#include <stdint.h>

typedef uint64_t FileSize;


class File
{
	public:
		enum Access { ACC_READ = 1, ACC_WRITE = 2, ACC_CREATE = 4 };
		enum SeekMode { FROM_START, FROM_CUR, FROM_END };

		virtual ~File() {}

		virtual int getHandle() const = 0;
		virtual FileSize getSize() const = 0;

//		virtual FileSize read( void *buf, FileSize length ) = 0;
//		virtual FileSize write( const void *buf, FileSize length ) = 0;
//		virtual FileSize seek( FileSize offset, SeekMode mode ) = 0;
		virtual void write( const void *data, FileSize length ) = 0;
};


#endif  // __DOS32_OS_FILE_H__
