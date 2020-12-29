
#ifndef __DOS32_OS_UNIX_FILE_H__
#define __DOS32_OS_UNIX_FILE_H__

#include <string>

#include "os/File.h"


namespace host {

class UnixFile : public File
{
	public:
		UnixFile( const std::string &fileName, int access );
		UnixFile( int fd );
		virtual ~UnixFile();

		virtual int getHandle() const;
		virtual FileSize getSize() const;

		virtual FileSize read( void *buf, FileSize length );
		virtual FileSize write( const void *buf, FileSize length );
		virtual FileSize seek( FileSize offset, SeekMode mode );

	private:
		int mFd;

		int convertFlags( int access );
};

}


#endif  // __DOS32_OS_UNIX_FILE_H__
