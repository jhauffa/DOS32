
#ifndef __DOS32_HOST_DIRECTORY_MAPPER__
#define __DOS32_HOST_DIRECTORY_MAPPER__

#include "Volume.h"


namespace host {
class File;
}

class HostDirectoryMapper : public Volume
{
	public:
		HostDirectoryMapper( const std::string &baseDirName );
		virtual ~HostDirectoryMapper() {}

		virtual void setCurrentPath( const std::string &pathName );
		virtual const std::string &getCurrentPath() const;

		virtual File *createFile( const std::string &fileName );

	private:
		const std::string mBaseDirName;
		std::string mCurPathName;
};

class HostFile : public File
{
	public:
		HostFile( const std::string &fileName );
		virtual ~HostFile();

		virtual size_t read( void *data, size_t size );
		virtual size_t write( const void *data, size_t size );
		virtual size_t seek( long offset, int mode );
		virtual uint16_t getDeviceFlags() const;

	private:
		host::File *mFile;
};


#endif	// __DOS32_HOST_DIRECTORY_MAPPER__
