
#include <sys/time.h>

#include "UnixException.h"
#include "UnixDateTime.h"


UnixTime::UnixTime() : mOffset( 0 )
{
	update();
}

UnixTime::~UnixTime()
{
}

void UnixTime::update()
{
	struct timeval rawTime;
	if ( gettimeofday( &rawTime, NULL ) == -1 )
		throw UnixException();
	rawTime.tv_sec += mOffset;
	mCurrentTime = localtime( &rawTime.tv_sec );
	mMilliSeconds = rawTime.tv_usec / 10000;
}

void UnixTime::setBase( unsigned int hours, unsigned int minutes, unsigned int seconds )
{
	// TODO: move into generic sytem clock class?
	struct tm baseTime;
	baseTime.tm_hour = hours;
	baseTime.tm_min = minutes;
	baseTime.tm_sec = seconds;
	mOffset = mktime( &baseTime ) - time( NULL );
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
