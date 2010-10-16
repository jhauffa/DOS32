
#ifndef __DOS32_OS_UNIX_DATE_TIME_H__
#define __DOS32_OS_UNIX_DATE_TIME_H__

#include <time.h>

#include "../DateTime.h"


class UnixTime : public Time
{
	public:
		UnixTime();
		virtual ~UnixTime();

		virtual void update();
		virtual void setBase( unsigned int hours, unsigned int minutes,
			unsigned int seconds );

		virtual unsigned int getHours() const;
		virtual unsigned int getMinutes() const;
		virtual unsigned int getSeconds() const;
		virtual unsigned int getMilliSeconds() const;

	private:
		int mOffset;
		struct tm *mCurrentTime;
		unsigned int mMilliSeconds;
};


#endif  // __DOS32_OS_UNIX_DATE_TIME_H__
