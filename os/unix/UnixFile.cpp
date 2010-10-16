
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "UnixException.h"
#include "UnixFile.h"


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

void UnixFile::write( const void *data, FileSize length )
{
	if ( ::write( mFd, data, length ) == -1 )
		throw UnixException();
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
