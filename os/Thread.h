
#ifndef __DOS32_OS_THREAD_H__
#define __DOS32_OS_THREAD_H__


namespace host {

typedef int (*ThreadProc)(void *);

class Thread
{
	public:
		virtual ~Thread() {}

		virtual void run() = 0;
		virtual int join() = 0;
};

}


#endif  // __DOS32_OS_THREAD_H__
