
#ifndef __DOS32_OS_UNIX_PATH_H__
#define __DOS32_OS_UNIX_PATH_H__

#include <string>

#include "os/Path.h"


namespace host {

class UnixPath : public Path
{
	public:
		UnixPath( const std::string &hostPrefix, const std::string &dosSuffix );
		virtual ~UnixPath() {}

		virtual bool exists() const;

	private:
		std::string mHostPath;
};

}


#endif  // __DOS32_OS_UNIX_PATH_H__
