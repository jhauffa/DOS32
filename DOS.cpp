
#include <cstring>
#include <cctype>

#include "os/OS.h"
#include "Debug.h"
#include "DOS.h"


DOS::DOS( int argc, char *argv[], char *envp[] ) :
	mLastError( DOSException::ERROR_NO_ERROR ), mVolumeManager()
{
	mTime = host::OS::createTime();

	// create standard file handles
	mOpenFiles.reserve( NUM_FILE_HANDLES );
	mOpenFiles.push_back( mVolumeManager.createConsole( stdin ) );
	mOpenFiles.push_back( mVolumeManager.createConsole( stdout ) );
	mOpenFiles.push_back( mVolumeManager.createConsole( stderr ) );
	mOpenFiles.push_back( NULL );  // TODO: STDAUX
	mOpenFiles.push_back( NULL );  // TODO: STDPRN

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
	mPsp->numFileHandles = NUM_FILE_HANDLES;
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

bool DOS::handleInterrupt( uint8_t idx, host::Context &ctx, void *lowMemBase )
{
	// called from DOSExtender::handleInterrupt
	assert( idx == 0x21 );

	bool canResume = true;

	uint8_t functionIdx = ctx.getAH();
	ctx.setCF( false );

	switch ( functionIdx )
	{
		case 0x0E:
			TRACE( "set current drive\n" );
			mVolumeManager.setCurrentDrive( ctx.getDL() );
			ctx.setAL( mVolumeManager.getMaxDrive() );
			break;
		case 0x19:
			TRACE( "get current drive\n" );
			ctx.setAL( mVolumeManager.getCurrentDrive() );
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
					ctx.getDX() ), ctx );
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
			TRACE( "seek\n" );
			fileSeek( ctx );
			break;
		case 0x44:
			TRACE( "IOCTL\n" );
			if ( ctx.getAL() != 0 )
			{
				FIXME( "sub-function 0x%02x not implemented\n", ctx.getAL() );
				canResume = false;
				break;
			}
			fileGetDeviceFlags( ctx );
			break;
		case 0x47:
			TRACE( "get current directory\n" );
			getCurrentDirectory( (char *) translateAddress( lowMemBase, ctx.getDS(),
					ctx.getSI() ), ctx );
			break;
		case 0x4C:
			TRACE( "exit, return code = %u\n", ctx.getAL() );
			host::OS::exitThread( ctx.getAL() );
			break;
		case 0x59:
			TRACE( "get extended error information\n" );
			if ( ctx.getBX() != 0 )
			{
				FIXME( "sub-function 0x%04x not implemented\n", ctx.getBX() );
				canResume = false;
				break;
			}
			ctx.setAX( mLastError.getErrorCode() );
			ctx.setBH( mLastError.getErrorClass() );
			ctx.setBL( mLastError.getRecommendedAction() );
			ctx.setCH( mLastError.getErrorLocus() );
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
}

void DOS::convertDOSException( const DOSException &ex, host::Context &ctx )
{
	mLastError = ex;
	ctx.setCF( true );
	ctx.setAX( ex.getErrorCode() );
}

uint8_t DOS::extractDrive( const std::string &pathName )
{
	if ( ( pathName.length() > 1 ) && ( pathName[1] == ':' ) )
		return (uint8_t) std::toupper( pathName[0] ) - 'A';
	return mVolumeManager.getCurrentDrive();
}

GuestFile *DOS::getOpenFile( uint16_t handle )
{
	if ( handle >= mOpenFiles.size() )
		throw DOSException( DOSException::ERROR_INVALID_HANDLE );
	GuestFile *f = mOpenFiles[handle];
	if ( !f )
		throw DOSException( DOSException::ERROR_INVALID_HANDLE );
	return f;
}

void DOS::setCurrentDirectory( char *path, host::Context &ctx )
{
	TRACE( "path = %s\n", path );
	try
	{
		mVolumeManager.getVolume( extractDrive( path ) ).setCurrentPath( path );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}

void DOS::getCurrentDirectory( char *path, host::Context &ctx )
{
	try
	{
		uint8_t drive = ctx.getDL();
		if ( drive == 0 )
			drive = mVolumeManager.getCurrentDrive();
		else
			drive--;

		const std::string &pathName = mVolumeManager.getVolume( drive ).getCurrentPath();
		strcpy( path, pathName.c_str() );
		ctx.setAX( 0x0100 );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}

void DOS::fileOpen( char *path, host::Context &ctx )
{
	TRACE( "path = %s\n", path );
	try
	{
		GuestFile *f =
			mVolumeManager.getVolume( extractDrive( path ) ).createFile( path );
		uint16_t handle = mOpenFiles.size();
		if ( handle >= NUM_FILE_HANDLES )
			throw DOSException( DOSException::ERROR_OUT_OF_HANDLES );
		mOpenFiles.push_back( f );
		ctx.setAX( handle );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}

void DOS::fileWrite( char *data, host::Context &ctx )
{
	try
	{
		GuestFile *f = getOpenFile( ctx.getBX() );
		// TODO: does DOS/4GW use ECX/EAX?
		ctx.setAX( f->write( data, ctx.getCX() ) & 0xFFFF );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}

void DOS::fileSeek( host::Context &ctx )
{
	try
	{
		GuestFile *f = getOpenFile( ctx.getBX() );
		size_t newPos = f->seek( ( ctx.getCX() << 16 ) | ctx.getDX(),
			(GuestFile::SeekMode) ctx.getAL() );
		ctx.setDX( ( newPos >> 16 ) & 0xFFFF );
		ctx.setAX( newPos & 0xFFFF );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}

void DOS::fileGetDeviceFlags( host::Context &ctx )
{
	try
	{
		GuestFile *f = getOpenFile( ctx.getBX() );
		ctx.setDX( f->getDeviceFlags() );
	}
	catch ( const DOSException &ex )
	{
		convertDOSException( ex, ctx );
	}
}
