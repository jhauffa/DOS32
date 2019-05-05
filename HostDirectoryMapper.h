
#ifndef __DOS32_HOST_DIRECTORY_MAPPER__
#define __DOS32_HOST_DIRECTORY_MAPPER__

#include "Volume.h"


class HostDirectoryMapper : public Volume
{
	public:
		HostDirectoryMapper( const std::string &baseDirName );
		virtual ~HostDirectoryMapper() {}

		virtual void setCurrentPath( const std::string &pathName );
		virtual const std::string &getCurrentPath() const;

		virtual File *createFile( const std::string &fileName );

	private:
		const std::string &mBaseDirName;
		std::string mCurPathName;
};


#endif	// __DOS32_HOST_DIRECTORY_MAPPER__
