
#ifndef __DOS32_VOLUME_H__
#define __DOS32_VOLUME_H__

#include <cstdio>
#include <cstdint>
#include <string>


class File
{
	public:
		virtual ~File() {}

		enum SeekMode
		{
			SEEK_MODE_SET = 0x00,
			SEEK_MODE_CUR = 0x01,
			SEEK_MODE_END = 0x02
		};

		virtual size_t write( void *data, size_t size ) = 0;
		virtual size_t seek( long offset, int mode ) = 0;
		virtual uint16_t getDeviceFlags() const = 0;
};

class ConsoleFile : public File
{
	public:
		ConsoleFile( FILE *stream );
		virtual ~ConsoleFile() {}

		virtual size_t write( void *data, size_t size );
		virtual size_t seek( long offset, int mode );
		virtual uint16_t getDeviceFlags() const;

	private:
		FILE *mStream;
};

enum DeviceFlagsCharacter
{
	HAS_CONTROL_CHANNEL = (1 << 14),
	IS_CHARACTER_DEVICE = (1 << 7),
	EOF_ON_INPUT = (1 << 6),
	IS_STANDARD_OUTPUT = (1 << 1),
	IS_STANDARD_INPUT = (1 << 0)
};

class Volume
{
	public:
		virtual ~Volume() {}

		virtual void setCurrentPath( const std::string &pathName ) = 0;
		virtual const std::string &getCurrentPath() const = 0;

		virtual File *createFile( const std::string &fileName ) = 0;
};

class VolumeManager
{
	public:
		VolumeManager();
		~VolumeManager();

		void setVolume( uint8_t drive, Volume *volume );
		void setCurrentDrive( uint8_t drive );
		uint8_t getCurrentDrive() const;
		uint8_t getMaxDrive() const;
		Volume &getCurrentVolume();
		Volume &getVolume( uint8_t drive );

		File *createConsole( FILE *stream );

	private:
		static const uint8_t MAX_DRIVE = 25;
		uint8_t mCurrentDrive;
		Volume *mVolumes[MAX_DRIVE + 1];
};


#endif	// __DOS32_VOLUME_H__
