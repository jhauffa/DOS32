
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "os/unix/UnixException.h"
#include "os/unix/UnixFile.h"


namespace host {

UnixFile::UnixFile( const std::string &fileName, int access ) : mFd( -1 )
{
	int flags = convertFlags( access );
	if ( flags & O_CREAT )
		mFd = open( fileName.c_str(), flags, S_IRUSR | S_IWUSR );
	else
		mFd = open( fileName.c_str(), flags );
	if ( mFd < 0 )
		throw UnixException();
}

UnixFile::UnixFile( int fd ) : mFd( fd )
{
}

UnixFile::~UnixFile()
{
	if ( mFd >= 0 )
		close( mFd );
}

int UnixFile::getHandle() const
{
	return mFd;
}

FileSize UnixFile::getSize() const
{
	struct stat statbuf;
	if ( fstat( mFd, &statbuf ) == -1 )
		throw UnixException();
	return statbuf.st_size;
}

FileSize UnixFile::read( void *buf, FileSize length )
{
	ssize_t bytesRead = ::read( mFd, buf, length );
	if ( bytesRead == -1 )
		throw UnixException();
	return bytesRead;
}

FileSize UnixFile::write( const void *buf, FileSize length )
{
	ssize_t bytesWritten = ::write( mFd, buf, length );
	if ( bytesWritten == -1 )
		throw UnixException();
	return bytesWritten;
}

FileSize UnixFile::seek( FileSize offset, SeekMode mode )	// XXX: offset should be unsigned
{
	int whence = 0;
	if ( mode == FROM_START )
		whence = SEEK_SET;
	else if ( mode == FROM_CUR )
		whence = SEEK_CUR;
	else if ( mode == FROM_END )
		whence = SEEK_END;
	off_t pos = ::lseek( mFd, offset, whence );
	if ( pos == -1 )
		throw UnixException();
	return pos;
}

int UnixFile::convertFlags( int access )
{
	int flags = 0;
	if ( ( access & ( ACC_READ | ACC_WRITE ) ) == ( ACC_READ | ACC_WRITE ) )
		flags = O_RDWR;
	else if ( access & ACC_READ )
		flags = O_RDONLY;
	else if ( access & ACC_WRITE )
		flags = O_WRONLY;
	if ( access & ACC_CREATE )
		flags |= O_CREAT;
	return flags;
}

}
