
#include <unistd.h>

#include "os/unix/UnixPath.h"


namespace host {

UnixPath::UnixPath( const std::string &hostPrefix, const std::string &dosSuffix ) :
	mHostPath( hostPrefix )
{
	mHostPath.append( dosSuffix );
	for ( int i = hostPrefix.length(); i < mHostPath.length(); i++ )
		if ( mHostPath[i] == '\\' )
			mHostPath[i] = '/';
}

bool UnixPath::exists() const
{
	return ( access( mHostPath.c_str(), F_OK ) == 0 );
}

}
