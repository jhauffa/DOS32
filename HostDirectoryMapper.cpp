
#include "os/OS.h"
#include "DOSException.h"
#include "HostDirectoryMapper.h"


HostDirectoryMapper::HostDirectoryMapper( const std::string &baseDirName ) :
		mBaseDirName( baseDirName ), mCurPathName( "" )
{
}

void HostDirectoryMapper::setCurrentPath( const std::string &pathName )
{
	const host::Path *hostPath = host::OS::createPath( mBaseDirName, pathName );
	bool exists = hostPath->exists();
	delete hostPath;
	if ( !exists )
		throw DOSException( DOSException::ERROR_PATH_NOT_FOUND );
	mCurPathName = pathName;
}

const std::string &HostDirectoryMapper::getCurrentPath() const
{
	return mCurPathName;
}

File *HostDirectoryMapper::createFile( const std::string &fileName )
{
	throw DOSException( DOSException::ERROR_FUNCTION_NOT_SUPPORTED );
}
