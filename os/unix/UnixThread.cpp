
#include "os/unix/UnixException.h"
#include "os/unix/UnixThread.h"


typedef void *(*PthreadProc)(void *);


UnixThread::UnixThread( ThreadProc p, void *data ) : mProc( p ), mData( data )
{
}

UnixThread::~UnixThread()
{
}

void UnixThread::run()
{
	int error = pthread_create( &mHandle, NULL, (PthreadProc) mProc, mData );
	if ( error != 0 )
		throw UnixException( error );	
}

int UnixThread::join()
{
	void *value;
	int error = pthread_join( mHandle, &value );
	if ( error != 0 )
		throw UnixException( error );
	return (int) value;
}

void UnixThread::exit( int result )
{
	pthread_exit( (void *) result );
}
