
#include "HostDirectoryMapper.h"
#include "DOSException.h"


HostDirectoryMapper::HostDirectoryMapper( const std::string &baseDirName ) :
		mBaseDirName( baseDirName ), mCurPathName( "" )
{
}

void HostDirectoryMapper::setCurrentPath( const std::string &pathName )
{
	// XXX: throw exception if path does not exist
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
