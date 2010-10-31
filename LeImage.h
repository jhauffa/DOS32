
#ifndef __DOS32_LE_IMAGE_H__
#define __DOS32_LE_IMAGE_H__

#include <vector>
#include <stdint.h>

#include "Image.h"


#define MAGIC_MZ		0x5A4D
#define MAGIC_LE		0x454C

#define DOS_EXT_TYPE		0x18
#define DOS_EXT_TYPE_MAGIC	0x40
#define DOS_LE_OFFSET		0x3C

#define MODULE_FLAGS_UNSUPPORTED	0xFFFF5CCF
#define MODULE_FLAGS_NOT_EXECUTABLE	0x0000A000
#define MODULE_FLAGS_PM				0x00000300
#define MODULE_FLAGS_NO_INT_FIXUP	0x00000010

#define OBJECT_FLAGS_UNSUPPORTED	0xFFFFCFA0
#define OBJECT_FLAGS_READ			0x00000001
#define OBJECT_FLAGS_WRITE			0x00000002
#define OBJECT_FLAGS_EXEC			0x00000004
#define OBJECT_FLAGS_DISCARD		0x00000010
#define OBJECT_FLAGS_32BIT			0x00002000

#define PAGE_TYPE_NORMAL	0

#define FIXUP_SOURCE_BYTE			0x0		// low byte
#define FIXUP_SOURCE_16BIT_SEGMENT	0x2		// 16 bit segment / selector
#define FIXUP_SOURCE_16BIT_POINTER	0x3		// 32 bit segment:offset pair
#define FIXUP_SOURCE_16BIT_OFFSET	0x5		// 16 bit offset
#define FIXUP_SOURCE_32BIT_FARPTR	0x6		// 48 bit selector:offset pair
#define FIXUP_SOURCE_32BIT_OFFSET	0x7		// 32 bit offset
#define FIXUP_SOURCE_32BIT_REL		0x8		// 32 bit offset ???
#define FIXUP_SOURCE_TYPE_MASK		0x0F
#define FIXUP_SOURCE_FLAGS_ALIAS	0x10
#define FIXUP_SOURCE_FLAGS_LIST		0x20

#define FIXUP_TARGET_INTERNAL		0x0
#define FIXUP_TARGET_TYPE_MASK		0x03
#define FIXUP_TARGET_FLAGS_ADD		0x04
#define FIXUP_TARGET_FLAGS_32BIT	0x10


struct LeHeader
{
	uint16_t magic;
	uint8_t byteOrder;
	uint8_t wordOrder;
	uint32_t formatLevel;
	uint16_t cpuType;
	uint16_t osType;
	uint32_t moduleVersion;
	uint32_t moduleFlags;
	uint32_t numModulePages;
	uint32_t eipObjectIdx;  // 1 based
	uint32_t eip;
	uint32_t espObjectIdx;  // 1 based
	uint32_t esp;
	uint32_t pageSize;
	uint32_t bytesLastPage;
	uint32_t fixupSectionSize;
	uint32_t fixupSectionChecksum;
	uint32_t loaderSectionSize;
	uint32_t loaderSectionChecksum;
	uint32_t objectTableOffset;
	uint32_t numObjects;
	uint32_t pageTableOffset;
	uint32_t objectIterPagesOffset;
	uint32_t resourceTableOffset;
	uint32_t numResources;
	uint32_t residentNameTableOffset;
	uint32_t entryTableOffset;
	uint32_t moduleDirectivesOffset;
	uint32_t numModuleDirectives;
	uint32_t fixupPageTableOffset;
	uint32_t fixupRecordTableOffset;
	uint32_t importModuleTableOffset;
	uint32_t numImportModules;
	uint32_t importProcTableOffset;
	uint32_t pageChecksumOffset;
	uint32_t dataPagesOffset;
	uint32_t numPreloadPages;
	uint32_t nonResidentNameTableOffset;
	uint32_t nonResidentNameTableLength;
	uint32_t nonResidentNameTableChecksum;
	uint32_t numAutoDsObjects;
	uint32_t debugInfoOffset;
	uint32_t debugInfoLength;
	uint32_t numInstancePreload;
	uint32_t numInstanceDemand;
	uint32_t heapSize;
} __attribute__ ((packed));

struct LeObjectTableEntry
{
	uint32_t virtualSize;
	uint32_t relocBaseAddr;
	uint32_t flags;
	uint32_t pageTableIdx;  // 1 based
	uint32_t numPageTableEntries;
	uint32_t reserved;
} __attribute__ ((packed));

struct LePageTableEntry
{
	uint16_t pageHigh;
	uint8_t pageLow;
	uint8_t type;
} __attribute__ ((packed));

typedef uint32_t LeFixupTableEntry;

struct LeFixupRecord
{
	uint8_t sourceType;
	uint8_t targetType;
	uint8_t data[1];  // variable size
} __attribute__ ((packed));


class MemMap;

class LeImage : public Image
{
	public:
		LeImage( const std::string &fileName, uint32_t maxHeapSize );
		virtual ~LeImage();

		virtual void load();

		virtual void *getEntryPoint() const;
		virtual void *getStackPointer() const;
		virtual void *getHeapEnd() const;

	private:
		const File *mFile;
		void *mEntryPoint;
		void *mStackPointer;
		void *mHeapEnd;
		std::vector<MemMap *> mObjectMappings;
		uint16_t mCodeSel;

		LeHeader *findLeHeader( const MemMap &mem ) const;
		void verifyHeader( const LeHeader *header ) const;
		void mapObject( const MemMap &mem, const LeHeader *header,
			const LeObjectTableEntry *object, const LePageTableEntry *pageTable,
			bool isStack );
		void relocate( const MemMap &mem, const LeHeader *header,
			const LeObjectTableEntry *objectTable, const LeFixupTableEntry *fixupTable,
			const LeFixupRecord *fixupRecordTable );
		int processRelocationRecord( int objectIdx, uint32_t objectOffset,
			const LeFixupRecord *reloc );

		static uint32_t roundToPageSize( uint32_t size, uint32_t pageSize );
};


#endif  // __DOS32_LE_IMAGE_H__
