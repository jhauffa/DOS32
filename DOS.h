
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
		void setCurrentDirectory( const char *path, host::Context &ctx );
		void getCurrentDirectory( char *path, host::Context &ctx );
		void fileOpen( char *path, host::Context &ctx );
		void fileClose( host::Context &ctx );
		void fileRead( char *data, host::Context &ctx );
		void fileWrite( const char *data, host::Context &ctx );
		void fileSeek( host::Context &ctx );
		void fileGetDeviceFlags( host::Context &ctx );

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
		void convertDOSException( const DOSException &ex, host::Context &ctx );
		uint8_t extractDrive( const char *pathName, const char **pathSuffix );

		static const uint8_t NUM_FILE_HANDLES = 20;

		static void *translateAddress( void *base, uint16_t segment, uint16_t offset );
};


#endif  // __DOS32_DOS_H__
