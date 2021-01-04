
#include <cassert>

#include "Volume.h"
#include "HostDirectoryMapper.h"
#include "DOSException.h"


ConsoleFile::ConsoleFile( FILE *stream ) : mStream( stream )
{
}

size_t ConsoleFile::read( void *data, size_t size )
{
	return fread( data, 1, size, mStream );
}

size_t ConsoleFile::write( const void *data, size_t size )
{
	return fwrite( data, 1, size, mStream );
}

size_t ConsoleFile::seek( long offset, int mode )
{
	throw DOSException( DOSException::ERROR_FUNCTION_NOT_SUPPORTED );
}

uint16_t ConsoleFile::getDeviceFlags() const
{
	return ( HAS_CONTROL_CHANNEL | IS_CHARACTER_DEVICE | EOF_ON_INPUT |
		IS_STANDARD_OUTPUT | IS_STANDARD_INPUT );
}


VolumeManager::VolumeManager()
{
	for ( int i = 0; i < MAX_DRIVE; i++ )
		mVolumes[i] = NULL;

	// TODO: configuration should be read from file
	setVolume( 2, new HostDirectoryMapper( "." ) );
	setCurrentDrive( 2 );
}

VolumeManager::~VolumeManager()
{
	for ( int i = 0; i < MAX_DRIVE; i++ )
		delete mVolumes[i];
}

void VolumeManager::setVolume( uint8_t drive, Volume *volume )
{
	assert( drive <= MAX_DRIVE );
	delete mVolumes[drive];
	mVolumes[drive] = volume;
}

void VolumeManager::setCurrentDrive( uint8_t drive )
{
	// keep current drive if new one is invalid
	if ( ( drive <= MAX_DRIVE ) && mVolumes[drive] )
		mCurrentDrive = drive;
}

uint8_t VolumeManager::getCurrentDrive() const
{
	return mCurrentDrive;
}

uint8_t VolumeManager::getMaxDrive() const
{
	return MAX_DRIVE;
}

Volume &VolumeManager::getCurrentVolume()
{
	return getVolume( mCurrentDrive );
}

Volume &VolumeManager::getVolume( uint8_t drive )
{
	if ( ( drive > MAX_DRIVE ) || ( !mVolumes[drive] ) )
		throw DOSException( DOSException::ERROR_INVALID_DRIVE );
	return *mVolumes[drive];
}

File *VolumeManager::createConsole( FILE *stream )
{
	return new ConsoleFile( stream );
}
