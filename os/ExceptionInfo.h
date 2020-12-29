
#ifndef __DOS32_OS_EXCEPTION_INFO_H__
#define __DOS32_OS_EXCEPTION_INFO_H__

namespace host {

class Context;


class ExceptionInfo
{
	public:
		virtual ~ExceptionInfo() {}
		
		virtual void *getFaultAddr() const = 0;
		virtual const Context &getContext() const = 0;
		virtual Context &getMutableContext() = 0;

		virtual void dump() const;
};

}


#endif  // __DOS32_OS_EXCEPTION_INFO_H__
