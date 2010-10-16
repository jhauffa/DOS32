
#include <string>
#include <stdio.h>

#include "Exception.h"
#include "Image.h"
#include "ExecutionEnvironment.h"
#include "DOS.h"
#include "DOSExtender.h"
#include "DPMI.h"

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

	ExecutionEnvironment env;
	DOS dosServices( argc, argv, envp );
	DOSExtender dosExtenderServices( dosServices );
	DPMI dpmiServices;

	int result;
	try
	{
		Image *img = ImageFactory::create( std::string( argv[1] ), MAX_HEAP_SIZE );
		result = env.run( img );
		delete img;
	}
	catch ( const Exception &ex )
	{
		fprintf( stderr, "%s\n", ex.getErrorMessage().c_str() );
		result = 2;
	}

	return result;
}
