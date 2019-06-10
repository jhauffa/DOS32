
#include <string>
#include <cstdio>

#include "Exception.h"
#include "Image.h"
#include "ExecutionEnvironment.h"

// DOS/4GW could not use more than 64 MB RAM, so it is unlikely that any application
// will require more than 64 MB of heap space.
#define MAX_HEAP_SIZE  ( 64 * 1024 * 1024 )


int main( int argc, char *argv[], char *envp[] )
{
	if ( argc < 2 )
	{
		fprintf( stderr, "not enough arguments\n" );
		return 1;
	}

	int result = 0;
	Image *img = NULL;
	try
	{
		ExecutionEnvironment env( argc, argv, envp );
		img = ImageFactory::create( std::string( argv[1] ), MAX_HEAP_SIZE );
		result = env.run( img );
	}
	catch ( const Exception &ex )
	{
		fprintf( stderr, "%s\n", ex.getErrorMessage().c_str() );
		result = 2;
	}
	delete img;
	return result;
}
