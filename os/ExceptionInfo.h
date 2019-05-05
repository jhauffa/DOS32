
#ifndef __DOS32_OS_EXCEPTION_INFO_H__
#define __DOS32_OS_EXCEPTION_INFO_H__

class Context;


class ExceptionInfo
{
	public:
		virtual ~ExceptionInfo() {}
		
		virtual void *getFaultAddr() = 0;
		virtual Context &getContext() = 0;

		virtual void dump();
};


#endif  // __DOS32_OS_EXCEPTION_INFO_H__
