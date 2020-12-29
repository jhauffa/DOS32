
#ifndef __DOS32_OS_DATE_TIME_H__
#define __DOS32_OS_DATE_TIME_H__


namespace host {

class Time
{
	public:
		virtual ~Time() {}

		virtual void update() = 0;
		virtual void setBase( unsigned int hours, unsigned int minutes,
			unsigned int seconds, unsigned int milliSeconds ) = 0;

		virtual unsigned int getHours() const = 0;
		virtual unsigned int getMinutes() const = 0;
		virtual unsigned int getSeconds() const = 0;
		virtual unsigned int getMilliSeconds() const = 0;
};

}


#endif  // __DOS32_OS_DATE_TIME_H__
