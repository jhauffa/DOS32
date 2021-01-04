
#ifndef __DOS32_OS_PATH_H__
#define __DOS32_OS_PATH_H__

#include <string>


namespace host {

class Path
{
	public:
		virtual ~Path() {}

		virtual bool exists() const = 0;
		virtual std::string getPathName() const = 0;
};

}


#endif  // __DOS32_OS_PATH_H__
