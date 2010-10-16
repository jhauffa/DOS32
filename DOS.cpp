
#include <string.h>

#include "Debug.h"
#include "ExecutionEnvironment.h"
#include "DOS.h"


DEFINE_INSTANCE( DOS );


DOS::DOS( int argc, char *argv[], char *envp[] ) :
	Singleton<DOS>( this )
{
	// create standard file handles
	mOpenFiles.reserve( 20 );
	mOpenFiles.push_back( OS::createStdInFile() );
	mOpenFiles.push_back( OS::createStdOutFile() );
	mOpenFiles.push_back( OS::createStdErrFile() );
	mOpenFiles.push_back( NULL );  // TODO: STDAUX
	mOpenFiles.push_back( NULL );  // TODO: STDPRN

	mTime = OS::createTime();

	// TODO: convert path names, filter environment variables
	initPsp( argc, argv );
	initEnvironment( envp, argv[1] );
}

DOS::~DOS()
{
	delete[] mEnvironment;
	delete mPsp;
	delete mTime;

	// TODO: empty file state to eschew need for pointers?
	for ( std::vector<File *>::iterator it = mOpenFiles.begin(); it != mOpenFiles.end();
	      ++it )
		delete *it;
}

PSP *DOS::getPsp() const
{
	return mPsp;
}

char *DOS::getEnvironment() const
{
	return mEnvironment;
}

uint32_t DOS::getEnvironmentSize() const
{
	return mEnvironmentSize;
}

void DOS::initPsp( int argc, char *argv[] )
{
	mPsp = new PSP;
	memset( mPsp, 0, sizeof( PSP ) );

	mPsp->callInt20 = 0x20CD;
	mPsp->callDosFar[0] = 0xCC;
	mPsp->numFileHandles = 20;
	mPsp->fileHandleTablePtr = 0x12;
	mPsp->callInt21Retf[0] = 0xCD;
	mPsp->callInt21Retf[1] = 0x21;
	mPsp->callInt21Retf[2] = 0xCB;

	// copy command line
	int pos = 1;
	for ( int i = 2; i < argc; i++ )
	{
		int len = strlen( argv[i] );
		if ( len > ( 125 - pos ) )
			break;
		mPsp->commandLine[pos++] = ' ';
		strcpy( &mPsp->commandLine[pos], argv[i] );
		pos += len;
	}
	mPsp->commandLine[pos++] = '\r';
	mPsp->commandLine[pos] = '\n';
	mPsp->commandLine[0] = pos - 1;
}

void DOS::initEnvironment( char *envp[], const char *appName )
{
	const char *p = *envp;
	while ( *p != '\0' )
	{
		int len = strlen( p ) + 1;
		p += len;
	}
	uint32_t unixEnvSize = ( p - *envp ) + 1;

	// format: n 0-terminated env strings, 0x00, 0x0001, 0-terminated program name
	mEnvironmentSize = unixEnvSize + 2 + strlen( appName ) + 1;
	mEnvironment = new char[mEnvironmentSize];
	memcpy( mEnvironment, *envp, unixEnvSize );
	mEnvironment[unixEnvSize] = 0x01;
	mEnvironment[unixEnvSize + 1] = 0x00;
	strcpy( &mEnvironment[unixEnvSize + 2], appName );
}

bool DOS::int21Handler( uint8_t idx, Context &ctx )
{
	// called from DOSExtender::int21Handler

	bool canResume = true;

	uint8_t functionIdx = ctx.getAH();
	ctx.setCF( false );

	switch ( functionIdx )
	{
		case 0x19:
			TRACE( "get current default drive\n" );
			ctx.setAL( 0x02 );  // drive C
			break;
		case 0x2C:
		{
			TRACE( "get system time\n" );
			Time *time = getInstance().mTime;
			time->update();
			ctx.setCH( time->getHours() );
			ctx.setCL( time->getMinutes() );
			ctx.setDH( time->getSeconds() );
			ctx.setDL( time->getMilliSeconds() * 10 );
			break;
		}
		case 0x2D:
		{
			TRACE( "set system time\n" );
			Time *time = getInstance().mTime;
			uint8_t hours = ctx.getCH();
			uint8_t minutes = ctx.getCL();
			uint8_t seconds = ctx.getDH();
			uint8_t centiSeconds = ctx.getDL();
			if ( ( hours < 24 ) && ( minutes < 60 ) && ( seconds < 60 ) &&
			     ( centiSeconds < 100 ) )
			{
				time->setBase( hours, minutes, seconds );
				ctx.setAL( 0 );
			}
			else
				ctx.setAL( 0xFF );
			break;
		}
		case 0x30:
			TRACE( "get version\n" );
			if ( ctx.getAL() == 0x01 )
				ctx.setBH( 0 );
			else
				ctx.setBH( 0xFF );
			ctx.setAX( 0x0005 );  // DOS 5.0

			// 24-bit serial number
			ctx.setBL( 0x12 );
			ctx.setCX( 0x3457 );
			break;
		case 0x3B:
			TRACE( "set current directory\n" );
			// TODO: hack, hooked by DOS extender
			TRACE( "new CWD = \"%s\"\n", (char *) ctx.getEDX() );
			ctx.setCF( true );
			break;
		case 0x3D:
			TRACE( "open\n" );
			// TODO: hack, hooked by DOS extender
			TRACE( "file name = \"%s\"\n", (char *) ctx.getEDX() );
			ctx.setAX( 0x0002 );  // file not found
			ctx.setCF( true );
			break;
		case 0x40:
		{
			TRACE( "write, device = %u: ", ctx.getBX() );
			char *data = (char *) ctx.getEDX();
			for (int i = 0; i < ctx.getCX(); i++ )
				TRACE( "%c", data[i] );
			ctx.setAX( ctx.getCX() );
			TRACE( "\n" );
			break;
		}
		case 0x42:
			TRACE( "lseek, device = %u\n", ctx.getBX() );
			ctx.setCF( true );
			ctx.setAX( 0x0006 );  // invalid handle
			break;
		case 0x44:
			TRACE( "IOCTL, function %d, device = %u\n", ctx.getAL(), ctx.getBX() );
			// HACK
			if ( ctx.getBX() < 3 )
				ctx.setDX( 0x40c3 );
			else
				canResume = false;
			break;
		case 0x47:
		{
			TRACE( "get current directory\n" );
			uint8_t drive = ctx.getDL();
			if ( ( drive == 0x00 ) || ( drive == 0x02 ) )
			{
				// TODO: hooked by DOS extender
				char *buf = (char *) ctx.getESI();
				strcpy( buf, "RIVA" );
				ctx.setAX( 0x0100 );
			}
			else
			{
				ctx.setAX( 0x000F );
				ctx.setCF( true );
			}
			break;
		}
		case 0x4C:
			TRACE( "exit, return code = %u\n", ctx.getAL() );
			OS::exitThread( ctx.getAL() );
			break;
		default:
			TRACE( "not implemented\n" );
			canResume = false;
	}

	return canResume;
}
