
#include <sys/time.h>

#include "os/unix/UnixException.h"
#include "os/unix/UnixDateTime.h"


namespace host {

UnixTime::UnixTime() : mOffsetSeconds( 0 ), mOffsetMilliSeconds( 0 )
{
}

void UnixTime::update()
{
	struct timeval rawTime;
	if ( gettimeofday( &rawTime, NULL ) == -1 )
		throw UnixException();
	rawTime.tv_sec += mOffsetSeconds;

	mMilliSeconds = ( rawTime.tv_usec / 10000 ) + mOffsetMilliSeconds;
	int carrySeconds = mMilliSeconds / 1000;
	if ( carrySeconds != 0 )
	{
		rawTime.tv_sec += carrySeconds;
		mMilliSeconds %= 1000;
	}

	mCurrentTime = localtime( &rawTime.tv_sec );
}

void UnixTime::setBase( unsigned int hours, unsigned int minutes, unsigned int seconds,
	unsigned int milliSeconds )
{
	struct tm baseTime;
	baseTime.tm_hour = hours;
	baseTime.tm_min = minutes;
	baseTime.tm_sec = seconds;
	mOffsetSeconds = mktime( &baseTime ) - mktime( mCurrentTime );
	mOffsetMilliSeconds = milliSeconds - mMilliSeconds;
}

unsigned int UnixTime::getHours() const
{
	return mCurrentTime->tm_hour;
}

unsigned int UnixTime::getMinutes() const
{
	return mCurrentTime->tm_min;
}

unsigned int UnixTime::getSeconds() const
{
	return ( mCurrentTime->tm_sec == 60 ) ? 0 : mCurrentTime->tm_sec;
}

unsigned int UnixTime::getMilliSeconds() const
{
	return mMilliSeconds;
}

}
