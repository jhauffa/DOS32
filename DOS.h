
#ifndef __DOS32_DOS_H__
#define __DOS32_DOS_H__

#include <stdint.h>
#include <vector>

#include "os/OS.h"
#include "InterruptHandler.h"


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


class DOS : public InterruptHandler
{
	public:
		DOS( int argc, char *argv[], char *envp[] );
		~DOS();

		virtual bool handleInterrupt( uint8_t idx, Context &ctx );

		PSP *getPsp() const;
		char *getEnvironment() const;
		uint32_t getEnvironmentSize() const;

	private:
		PSP *mPsp;
		char *mEnvironment;
		uint32_t mEnvironmentSize;
		Time *mTime;
		std::vector<File *> mOpenFiles;

		void initPsp( int argc, char *argv[] );
		void initEnvironment( char *envp[], const char *appName );
};


#endif  // __DOS32_DOS_H__
