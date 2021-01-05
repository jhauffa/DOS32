
#include <memory>

#include "os/OS.h"
#include "os/OSException.h"
#include "os/Path.h"
#include "os/File.h"
#include "DOSException.h"
#include "HostDirectoryMapper.h"


// TODO: create Path object for baseDirName; for each operation, check if absolute path /
//	file name is prefix of absolute base directory
HostDirectoryMapper::HostDirectoryMapper( const std::string &baseDirName ) :
		mBaseDirName( baseDirName ), mCurPathName( "" )
{
}

void HostDirectoryMapper::setCurrentPath( const std::string &pathName )
{
	const std::unique_ptr<host::Path> hostPath( host::OS::createPath( mBaseDirName,
		pathName ) );
	if ( !hostPath->exists() )
		throw DOSException( DOSException::ERROR_PATH_NOT_FOUND );
	mCurPathName = pathName;
}

const std::string &HostDirectoryMapper::getCurrentPath() const
{
	return mCurPathName;
}

File *HostDirectoryMapper::createFile( const std::string &fileName )
{
	std::string qualifiedFileName = mCurPathName + '\\' + fileName;
	const std::unique_ptr<host::Path> hostPath( host::OS::createPath( mBaseDirName,
		qualifiedFileName ) );
	return new HostFile( hostPath->getPathName() );
}


HostFile::HostFile( const std::string &fileName ) : mFile( NULL )
{
	try
	{
		// TODO: proper permissions handling
		mFile = host::OS::createFile( fileName,
			host::File::ACC_READ | host::File::ACC_WRITE );
	}
	catch ( const host::OSException &ex )
	{
		throw DOSException( ex );
	}
}

HostFile::~HostFile()
{
	delete mFile;
}

size_t HostFile::read( void *data, size_t size )
{
	return mFile->read( data, size );
}

size_t HostFile::write( const void *data, size_t size )
{
	return mFile->write( data, size );
}

size_t HostFile::seek( long offset, int mode )
{
	host::File::SeekMode m;
	switch ( mode )
	{
	default:
	case SEEK_MODE_SET:
		m = host::File::FROM_START;
		break;
	case SEEK_MODE_CUR:
		m = host::File::FROM_CUR;
		break;
	case SEEK_MODE_END:
		m = host::File::FROM_END;
		break;
	}
	return mFile->seek( offset, m );
}

uint16_t HostFile::getDeviceFlags() const
{
	// TODO: set bit 6 if any write access has taken place; bits 0-5 should contain drive
	//	number
	return ( NOT_REMOVABLE );
}
