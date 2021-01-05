
#ifndef __DOS32_DOS_H__
#define __DOS32_DOS_H__

#include <cstdint>

#include "DOSException.h"
#include "HandleTable.h"
#include "Volume.h"


namespace host {
class Context;
class Time;
}

struct PSP
{
	uint16_t callInt20;
	uint16_t nextParagraph;
	uint8_t reserved1;
	uint8_t callDosFar[5];
	uint32_t prevInt22Handler;
	uint32_t prevInt23Handler;
	uint32_t prevInt24Handler;
	uint16_t parentPspSegment;
	uint8_t openFileHandles[20];
	uint16_t environmentSegment;
	uint32_t lastInt21Stack;
	uint16_t numFileHandles;
	uint32_t fileHandleTablePtr;
	uint8_t reserved2[24];
	uint8_t callInt21Retf[3];
	uint8_t reserved3[9];
	uint8_t fcb1[16];
	uint8_t fcb2[20];
	char commandLine[128];  // Pascal string
} __attribute__ ((packed));


class DOS
{
	public:
		DOS( int argc, char *argv[], char *envp[] );
		~DOS();

		virtual bool handleInterrupt( uint8_t idx, host::Context &ctx, void *lowMemBase );

		PSP *getPsp() const;
		char *getEnvironment() const;
		uint32_t getEnvironmentSize() const;

		void setDTA( char *dta );
		void setCurrentDirectory( const char *path );
		void getCurrentDirectory( uint8_t drive, char *path );
		uint16_t fileOpen( const char *path );
		void fileClose( uint16_t handle );
		uint32_t fileRead( uint16_t handle, uint32_t n, char *data );
		uint32_t fileWrite( uint16_t handle, uint32_t n, const char *data );
		uint64_t fileSeek( uint16_t handle, uint64_t pos, uint8_t mode );
		uint16_t fileGetDeviceFlags( uint16_t handle );

	private:
		PSP *mPsp;
		char *mEnvironment;
		uint32_t mEnvironmentSize;
		host::Time *mTime;
		char *mDta;
		DOSException mLastError;
		VolumeManager mVolumeManager;
		HandleTable<File> mOpenFiles;

		void initPsp( int argc, char *argv[] );
		void initEnvironment( char *envp[], const char *appName );
		uint8_t extractDrive( const char *pathName, const char **pathSuffix );

		static const uint8_t NUM_FILE_HANDLES = 20;
};


#endif  // __DOS32_DOS_H__
