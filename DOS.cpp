
#include <cstring>

#include "Debug.h"
#include "DOS.h"


DOS::DOS( int argc, char *argv[], char *envp[] )
{
	mTime = OS::createTime();

	// TODO: convert/filter path names and env. variables (or load everything from cfg.)
	initPsp( argc, argv );
	mDta = ((char *) mPsp) + 0x80;
	initEnvironment( envp, argv[1] );
}

DOS::~DOS()
{
	delete[] mEnvironment;
	delete mPsp;
	delete mTime;
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

void *DOS::translateAddress( void *base, uint16_t segment, uint16_t offset )
{
	assert( base != NULL );
	return ((char *) base) + ((segment << 4) + offset);
}

bool DOS::handleInterrupt( uint8_t idx, Context &ctx, void *lowMemBase )
{
	// called from DOSExtender::handleInterrupt
	assert( idx == 0x21 );

	bool canResume = true;

	uint8_t functionIdx = ctx.getAH();
	ctx.setCF( false );

	switch ( functionIdx )
	{
		case 0x19:
			TRACE( "get current default drive\n" );
			FIXME( "stub\n" );
			ctx.setAL( 0x02 );  // drive C
			break;
		case 0x1A:
			TRACE( "set disk transfer address\n" );
			setDTA( (char *) translateAddress( lowMemBase, ctx.getDS(), ctx.getDX() ) );
			break;
		case 0x2C:
			TRACE( "get system time\n" );
			mTime->update();
			ctx.setCH( mTime->getHours() );
			ctx.setCL( mTime->getMinutes() );
			ctx.setDH( mTime->getSeconds() );
			ctx.setDL( mTime->getMilliSeconds() / 10 );
			break;
		case 0x2D:
		{
			TRACE( "set system time\n" );
			uint8_t hours = ctx.getCH();
			uint8_t minutes = ctx.getCL();
			uint8_t seconds = ctx.getDH();
			uint8_t centiSeconds = ctx.getDL();
			if ( ( hours < 24 ) && ( minutes < 60 ) && ( seconds < 60 ) &&
			     ( centiSeconds < 100 ) )
			{
				mTime->update();
				mTime->setBase( hours, minutes, seconds, centiSeconds * 10 );
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
			setCurrentDirectory( (char *) translateAddress( lowMemBase, ctx.getDS(),
					ctx.getDX() ), ctx) ;
			break;
		case 0x3D:
			TRACE( "open\n" );
			fileOpen( (char *) translateAddress( lowMemBase, ctx.getDS(), ctx.getDS() ),
				ctx );
			break;
		case 0x40:
			TRACE( "write\n" );
			fileWrite( (char *) translateAddress( lowMemBase, ctx.getDS(), ctx.getDS() ),
				ctx );
			break;
		case 0x42:
			TRACE( "lseek, device = %u\n", ctx.getBX() );
			FIXME( "stub\n" );
			ctx.setCF( true );
			ctx.setAX( 0x0006 );  // invalid handle
			break;
		case 0x44:
			TRACE( "IOCTL, function %d, device = %u\n", ctx.getAL(), ctx.getBX() );
			FIXME( "stub\n" );
			if ( ctx.getBX() < 3 )
				ctx.setDX( 0x40c3 );
			else
				canResume = false;
			break;
		case 0x47:
			TRACE( "get current directory\n" );
			getCurrentDirectory( (char *) translateAddress( lowMemBase, ctx.getDS(),
					ctx.getSI() ), ctx );
			break;
		case 0x4C:
			TRACE( "exit, return code = %u\n", ctx.getAL() );
			OS::exitThread( ctx.getAL() );
			break;
		default:
			FIXME( "not implemented\n" );
			canResume = false;
	}

	return canResume;
}

void DOS::setDTA( char *dta )
{
	mDta = dta;
	TRACE( "new DTA = %p\n", mDta );
}

void DOS::setCurrentDirectory( char *path, Context &ctx )
{
	FIXME( "stub\n" );
	TRACE( "new CWD = \"%s\"\n", path );
	ctx.setCF( true );
}

void DOS::getCurrentDirectory( char *path, Context &ctx )
{
	FIXME( "stub\n" );
	uint8_t drive = ctx.getDL();
	if ( ( drive == 0x00 ) || ( drive == 0x02 ) )
	{
		strcpy( path, "RIVA" );
		ctx.setAX( 0x0100 );
	}
	else
	{
		ctx.setAX( 0x000F );
		ctx.setCF( true );
	}
}

void DOS::fileOpen( char *filePath, Context &ctx )
{
	FIXME( "stub\n" );
	TRACE( "file name = \"%s\"\n", (char *) ctx.getEDX() );
	ctx.setAX( 0x0002 );  // file not found
	ctx.setCF( true );
}

void DOS::fileWrite( char *data, Context &ctx )
{
	FIXME( "stub\n" );
	TRACE( "device = %u: ", ctx.getBX() );
	for (int i = 0; i < ctx.getCX(); i++ )
		TRACE( "%c", data[i] );
	ctx.setAX( ctx.getCX() );
	TRACE( "\n" );
}
