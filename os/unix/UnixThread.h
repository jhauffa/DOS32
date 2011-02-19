
#ifndef __DOS32_OS_UNIX_THREAD_H__
#define __DOS32_OS_UNIX_THREAD_H__

#include <pthread.h>

#include "os/Thread.h"


class UnixThread : public Thread
{
	public:
		UnixThread( ThreadProc p, void *data );
		virtual ~UnixThread();

		virtual void run();
		virtual int join();

		static void exit( int result );

	private:
		pthread_t mHandle;
		ThreadProc mProc;
		void *mData;
};


#endif  // __DOS32_OS_UNIX_THREAD_H__
