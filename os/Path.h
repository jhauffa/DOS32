
#ifndef __DOS32_OS_PATH_H__
#define __DOS32_OS_PATH_H__


class Path
{
	public:
		virtual ~Path() {}

		virtual bool exists() const = 0;
};


#endif  // __DOS32_OS_PATH_H__
