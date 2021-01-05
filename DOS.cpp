
#include <cstring>
#include <cctype>

#include "os/OS.h"
#include "os/DateTime.h"
#include "os/Context.h"
#include "Debug.h"
#include "DOS.h"


DOS::DOS( int argc, char *argv[], char *envp[] ) :
	mLastError( DOSException::ERROR_NO_ERROR ), mOpenFiles( NUM_FILE_HANDLES ),
	mVolumeManager()
{
	mTime = host::OS::createTime();

	// create standard file handles
	mOpenFiles.allocate( mVolumeManager.createConsole( stdin ) );
	mOpenFiles.allocate( mVolumeManager.createConsole( stdout ) );
	mOpenFiles.allocate( mVolumeManager.createConsole( stderr ) );
	mOpenFiles.allocate( mVolumeManager.createConsole( stdout ) );  // TODO: STDAUX
	mOpenFiles.allocate( mVolumeManager.createConsole( stdout ) );  // TODO: STDPRN

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

bool DOS::handleInterrupt( uint8_t idx, host::Context &ctx, void *lowMemBase )
{
	// called from DOSExtender::handleInterrupt
	assert( idx == 0x21 );

	bool canResume = true;

	uint8_t functionIdx = ctx.getAH();
	ctx.setCF( false );

	try
	{
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
				setDTA( (char *) ctx.getEDX() );
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
				setCurrentDirectory( (const char *) ctx.getEDX() );
				break;
			case 0x3D:
				TRACE( "open\n" );
				ctx.setEAX( fileOpen( (const char *) ctx.getEDX() ) );
				break;
			case 0x3E:
				TRACE( "close\n" );
				fileClose( ctx.getBX() );
				break;
			case 0x3F:
				TRACE( "read\n" );
				ctx.setEAX( fileRead( ctx.getBX(), ctx.getECX(),
					(char *) ctx.getEDX() ) );
				break;
			case 0x40:
				TRACE( "write\n" );
				ctx.setEAX( fileWrite( ctx.getBX(), ctx.getECX(),
					(const char *) ctx.getEDX() ) );
				break;
			case 0x42:
			{
				TRACE( "seek\n" );
				uint64_t newPos = fileSeek( ctx.getBX(),
					( ( (uint64_t) ctx.getECX() << 32 ) & 0xFFFFFFFF00000000) |
						ctx.getEDX(), ctx.getAL() );
				ctx.setEDX( ( newPos >> 32 ) & 0xFFFFFFFF );
				ctx.setEAX( newPos & 0xFFFFFFFF );
				break;
			}
			case 0x44:
				TRACE( "IOCTL\n" );
				if ( ctx.getAL() != 0 )
				{
					FIXME( "sub-function 0x%02x not implemented\n", ctx.getAL() );
					canResume = false;
					break;
				}
				ctx.setDX( fileGetDeviceFlags( ctx.getBX() ) );
				break;
			case 0x47:
				TRACE( "get current directory\n" );
				getCurrentDirectory( ctx.getDL(), (char *) ctx.getESI() );
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
	}
	catch ( const DOSException &ex )
	{
		mLastError = ex;
		ctx.setCF( true );
		ctx.setEAX( ex.getErrorCode() );
	}

	return canResume;
}

void DOS::setDTA( char *dta )
{
	mDta = dta;
}

uint8_t DOS::extractDrive( const char *pathName, const char **pathSuffix )
{
	if ( ( strlen( pathName ) > 1 ) && ( pathName[1] == ':' ) )
	{
		*pathSuffix = pathName + 2;
		return (uint8_t) toupper( pathName[0] ) - 'A';
	}

	*pathSuffix = pathName;
	return mVolumeManager.getCurrentDrive();
}

void DOS::setCurrentDirectory( const char *path )
{
	const char *pathSuffix;
	uint8_t drive = extractDrive( path, &pathSuffix );
	mVolumeManager.getVolume( drive ).setCurrentPath( pathSuffix );
}

void DOS::getCurrentDirectory( uint8_t drive, char *path )
{
	if ( drive == 0 )
		drive = mVolumeManager.getCurrentDrive();
	else
		drive--;

	const std::string &pathName = mVolumeManager.getVolume( drive ).getCurrentPath();
	const char *p = pathName.c_str();
	if ( *p == '\\' )
		p++;
	strcpy( path, p );
}

uint16_t DOS::fileOpen( const char *path )
{
	const char *pathSuffix;
	uint8_t drive = extractDrive( path, &pathSuffix );
	File *f = mVolumeManager.getVolume( drive ).createFile( pathSuffix );
	return mOpenFiles.allocate( f );
}

void DOS::fileClose( uint16_t handle )
{
	delete mOpenFiles.release( handle );
}

uint32_t DOS::fileRead( uint16_t handle, uint32_t n, char *data )
{
	File *f = mOpenFiles.get( handle );
	return f->read( data, n );
}

uint32_t DOS::fileWrite( uint16_t handle, uint32_t n, const char *data )
{
	File *f = mOpenFiles.get( handle );
	return f->write( data, n );
}

uint64_t DOS::fileSeek( uint16_t handle, uint64_t pos, uint8_t mode )
{
	File *f = mOpenFiles.get( handle );
	return f->seek( pos, (File::SeekMode) mode );
}

uint16_t DOS::fileGetDeviceFlags( uint16_t handle )
{
	File *f = mOpenFiles.get( handle );
	return f->getDeviceFlags();
}
