
#ifndef __DOS32_OS_UNIX_DARWIN_EXCEPTION_INFO_H__
#define __DOS32_OS_UNIX_DARWIN_EXCEPTION_INFO_H__

#include <signal.h>

#include "os/ExceptionInfo.h"


class DarwinExceptionInfo : public ExceptionInfo
{
	public:
		DarwinExceptionInfo( int sig, siginfo_t *info, void *data );
		virtual ~DarwinExceptionInfo();

		virtual void *getFaultAddr() const;
		virtual const Context &getContext() const;
		virtual Context &getMutableContext();

		virtual void dump() const;

	private:
		int mSignal;
		siginfo_t *mInfo;
		Context *mContext;
};


#endif  // __DOS32_OS_UNIX_DARWIN_EXCEPTION_INFO_H__
