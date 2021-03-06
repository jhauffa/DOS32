
#include "os/OS.h"
#include "os/OSException.h"
#include "os/File.h"
#include "os/MemMap.h"
#include "ExecutionEnvironment.h"
#include "DescriptorTable.h"
#include "ImageLoaderException.h"
#include "Debug.h"
#include "LEImage.h"


namespace {

// register with ImageFactory

static Image *createLEImage( const std::string &fileName, uint32_t maxHeapSize )
{
	return new LEImage( fileName, maxHeapSize );
}

static ImageFactory creator( createLEImage );

}


LEImage::LEImage( const std::string &fileName, uint32_t maxHeapSize ) :
	Image( fileName, maxHeapSize ), mFile( NULL ), mEntryPoint( NULL ),
	mStackPointer( NULL ), mHeapEnd( NULL )
{
	ExecutionEnvironment &env = ExecutionEnvironment::getInstance();
	const DescriptorTable &descTable = env.getDescriptorTable();
	mCodeSel = descTable.getOSCodeSel();
}

LEImage::~LEImage()
{
	for ( std::vector<host::MemMap *>::iterator it = mObjectMappings.begin();
		  it != mObjectMappings.end(); ++it )
		delete *it;
	delete mFile;
}

void LEImage::load()
{
	TRACE( "LEImage: loading \"%s\"\n", mFileName.c_str() );
	host::MemMap *mem;
	try
	{
		mFile = host::OS::createFile( mFileName, host::File::ACC_READ );
		mem = host::OS::createMemMap( *mFile,
			host::MemMap::ACC_READ | host::MemMap::ACC_WRITE );
	}
	catch ( const host::OSException &ex )
	{
		throw ImageLoaderException( ex );
	}

	LEHeader *header = findLEHeader( *mem );
	if ( !header )
		throw ImageLoaderException( ImageLoaderException::IMG_UNKNOWN_FORMAT );
	verifyHeader( header );

	// locate object table, page table, fixup table and fixup record table
	// TODO: move into helper function, merge try-catch-blocks
	uint8_t *headerPtr = (uint8_t *) header;
	const LEPageTableEntry *pageTable = NULL;
	if ( header->pageTableOffset != 0 )
		pageTable = (LEPageTableEntry *) ( headerPtr + header->pageTableOffset );
	if ( !pageTable || !mem->isInRange( (void *) pageTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LEObjectTableEntry *objectTable = NULL;
	if ( ( header->objectTableOffset != 0 ) && ( header->numObjects > 0 ) )
		objectTable = (LEObjectTableEntry *) ( headerPtr + header->objectTableOffset );
	if ( !objectTable || !mem->isInRange( (void *) &objectTable[header->numObjects] ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LEFixupTableEntry *fixupTable = NULL;
	if ( header->fixupPageTableOffset != 0 )
		fixupTable = (LEFixupTableEntry *) ( headerPtr + header->fixupPageTableOffset );
	if ( !fixupTable || !mem->isInRange( (void *) fixupTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LEFixupRecord *fixupRecordTable = NULL;
	if ( header->fixupRecordTableOffset != 0 )
		fixupRecordTable = (LEFixupRecord *) (headerPtr + header->fixupRecordTableOffset);
	if ( !fixupRecordTable || !mem->isInRange( (void *) fixupRecordTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	// load objects into memory
	try
	{
		for (int i = 0; i < header->numObjects; i++)
		{
			bool isStack = ( i == ( header->espObjectIdx - 1 ) );
			mapObject( *mem, header, &objectTable[i], pageTable, isStack );
		}

		relocate( *mem, header, objectTable, fixupTable, fixupRecordTable );
	}
	catch ( const host::OSException &ex )
	{
		delete mem;
		throw ImageLoaderException( ex );
	}

	mEntryPoint =
		(void *) ( (uint8_t *) mObjectMappings[header->eipObjectIdx - 1]->getPtr() +
			header->eip );
	mStackPointer =
		(void *) ( (uint8_t *) mObjectMappings[header->espObjectIdx - 1]->getPtr() +
			header->esp );
	mHeapEnd = (uint8_t *) mStackPointer + mMaxHeapSize;
	delete mem;
}

void *LEImage::getEntryPoint() const
{
	return mEntryPoint;
}

void *LEImage::getStackPointer() const
{
	return mStackPointer;
}

void *LEImage::getHeapEnd() const
{
	return mHeapEnd;
}

LEHeader *LEImage::findLEHeader( const host::MemMap &mem ) const
{
	uint16_t *ptr = (uint16_t *) mem.getPtr();
	while ( mem.isInRange( ptr + ( DOS_EXT_TYPE / 2 ) + 1 ) )
	{
		if ( ( ptr[0] == MAGIC_MZ ) && ( ptr[DOS_EXT_TYPE / 2] == DOS_EXT_TYPE_MAGIC ) )
		{
			uint32_t headerOffset = *( (uint32_t *) ( (uint8_t *) ptr + DOS_LE_OFFSET ) );
			TRACE( "LE header offset = 0x%x\n", headerOffset );

			LEHeader *header = (LEHeader *) ( (uint8_t *) ptr + headerOffset );
			if ( mem.isInRange( (void *) ( header+1 ) ) && ( header->magic == MAGIC_LE ) )
			{
				TRACE( "found valid LE header\n" );
				header->dataPagesOffset += (uint32_t) ptr - (uint32_t) mem.getPtr();
				return header;
			}
			else
				ERR( "found invalid LE header\n" );
		}
		ptr++;
	}
	return NULL;
}

void LEImage::verifyHeader( const LEHeader *header ) const
{
	if ( ( header->byteOrder != 0 ) || ( header->wordOrder != 0 ) )
		throw ImageLoaderException( ImageLoaderException::IMG_INCOMPATIBLE );
	if ( header->formatLevel > 0 )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	if ( ( header->cpuType < 2 ) || ( header->cpuType > 4 ) )  // Intel 386 - 586
		throw ImageLoaderException( ImageLoaderException::IMG_INCOMPATIBLE );
	// ignore OS type, which is sometimes set to 0x01 (OS/2)
	if ( ( header->moduleFlags & MODULE_FLAGS_UNSUPPORTED ) != 0 )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	if ( ( header->moduleFlags & MODULE_FLAGS_NOT_EXECUTABLE ) != 0 )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_EXECUTABLE );
	if ( ( header->moduleFlags & MODULE_FLAGS_PM ) == MODULE_FLAGS_PM )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	if ( ( header->moduleFlags & MODULE_FLAGS_NO_INT_FIXUP ) != 0 )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_RELOCATABLE );

	if ( ( header->numResources > 0 )                ||
	     ( header->numModuleDirectives > 0 )         ||
	     ( header->numImportModules > 0 )            ||
	     ( header->nonResidentNameTableLength > 0 ) )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
}

void LEImage::mapObject( const host::MemMap &mem, const LEHeader *header,
	const LEObjectTableEntry *object, const LEPageTableEntry *pageTable, bool isStack )
{
	TRACE( "object base = 0x%x, size = %u, flags = 0x%x\n", object->relocBaseAddr,
		object->virtualSize, object->flags );

	if ( object->flags & OBJECT_FLAGS_DISCARD )
	{
		mObjectMappings.push_back( NULL );
		return;
	}

	if ( object->flags & OBJECT_FLAGS_UNSUPPORTED )
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );

	if (  ( object->flags & OBJECT_FLAGS_EXEC ) &&
	     !( object->flags & OBJECT_FLAGS_32BIT ) )
		FIXME( "16-bit code object\n" );

	if ( !mem.isInRange( (void *) &pageTable[object->pageTableIdx +
			object->numPageTableEntries] ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	// Allocate a VM area; ignore permission flags so that code can be patched without
	// having to modify and restore the read-only state.
	// TODO: evaluate cost/benefit/feasability of correct page protection
	host::MemSize allocSize = roundToPageSize( object->virtualSize, header->pageSize );
	if ( isStack )
		allocSize += mMaxHeapSize;
	host::MemMap *objectMapping = host::OS::createMemMap( allocSize,
		host::MemMap::ACC_READ | host::MemMap::ACC_WRITE | host::MemMap::ACC_EXEC );
	TRACE( "new object base = %p\n", objectMapping->getPtr() );
	mObjectMappings.push_back( objectMapping );

	// find and map contiguous regions
	uint32_t startOffset = 0;
	int startIdx = 0;
	uint32_t endOffset = 0;
	for ( int i = 0; i < object->numPageTableEntries; i++ )
	{
		const LEPageTableEntry *page = &pageTable[object->pageTableIdx + i - 1];
		if ( page->type != PAGE_TYPE_NORMAL )
			throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );

		uint32_t pageFileOffset = ( page->pageHigh + page->pageLow - 1 ) *
			header->pageSize + header->dataPagesOffset;

		if ( startOffset == 0 )
		{
			startOffset = pageFileOffset;
			startIdx = i;
		}
		if ( ( ( endOffset != 0 ) &&
		       ( pageFileOffset != ( endOffset + header->pageSize ) ) ) ||
		     ( i == ( object->numPageTableEntries - 1 ) ) )
		{
			endOffset = pageFileOffset + header->pageSize;
			uint32_t length = std::min( endOffset - startOffset, object->virtualSize );
			uint32_t objectOffset = startIdx * header->pageSize;
			TRACE( "mapping %u bytes from file offset 0x%x to object offset 0x%x\n",
				length, startOffset, objectOffset );
			objectMapping->map( *mFile, objectOffset, startOffset, length );
			startOffset = endOffset = 0;
		}
		else
			endOffset = pageFileOffset;
	}
}

void LEImage::relocate( const host::MemMap &mem, const LEHeader *header,
	const LEObjectTableEntry *objectTable, const LEFixupTableEntry *fixupTable,
	const LEFixupRecord *fixupRecordTable )
{
	for ( int i = 0; i < header->numObjects; i++ )
	{
		int firstPage = objectTable[i].pageTableIdx - 1;
		if ( !mem.isInRange(
				(void *) &fixupTable[firstPage + objectTable[i].numPageTableEntries] ) )
			throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

		for ( int j = 0; j < objectTable[i].numPageTableEntries; j++ )
		{
			const LEFixupRecord *relocStart = (const LEFixupRecord *)
				( (uint8_t *) fixupRecordTable + fixupTable[firstPage + j] );
			const LEFixupRecord *relocEnd =	(const LEFixupRecord *)
				( (uint8_t *) fixupRecordTable + fixupTable[firstPage + j + 1] );
			if ( !mem.isInRange( (void *) relocEnd ) )
				throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

			while ( relocStart < relocEnd )
			{
				int size = processRelocationRecord( i, j * header->pageSize, relocStart );
				relocStart = (const LEFixupRecord *) ( (uint8_t *) relocStart + size );
			}
		}
	}
}

int LEImage::processRelocationRecord( int objectIdx, uint32_t objectOffset,
	const LEFixupRecord *reloc )
{
	if ( reloc->sourceType & FIXUP_SOURCE_FLAGS_LIST )
	{
		FIXME( "relocation source list\n" );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}
	if ( ( reloc->targetType & FIXUP_TARGET_TYPE_MASK ) != FIXUP_TARGET_INTERNAL )
	{
		FIXME( "unknown relocation type, target 0x%x\n", reloc->targetType );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}
	if ( ( reloc->targetType & FIXUP_TARGET_FLAGS_ADD ) != 0 )
	{
		FIXME( "additive relocation\n" );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}

	int16_t pageOffset = *( (int16_t *) &reloc->data[0] );
	uint8_t *sourcePtr = (uint8_t *) mObjectMappings[objectIdx]->getPtr() +	objectOffset +
		pageOffset;
	uint8_t targetObjectIdx = reloc->data[2];
	TRACE( "relocation at %p: target object %u, ", sourcePtr, targetObjectIdx );
	uint32_t targetOffset = (uint32_t) mObjectMappings[targetObjectIdx - 1]->getPtr();
	// record size = source/target type (2 bytes) + page offset (2 bytes) + target object
	//                 index (1-based, 1 byte) + target offset (see below)
	int recordSize = 5;
	if ( reloc->targetType & FIXUP_TARGET_FLAGS_32BIT )
	{
		targetOffset += *( (uint32_t *) &reloc->data[3] );
		recordSize += 4;
	}
	else
	{
		targetOffset += *( (uint16_t *) &reloc->data[3] );
		recordSize += 2;
	}
	TRACE( "target offset = 0x%x\n", targetOffset );

	switch ( reloc->sourceType & FIXUP_SOURCE_TYPE_MASK )
	{
		case FIXUP_SOURCE_BYTE:
			FIXME( "byte relocation at %p, ignoring\n", sourcePtr );
			break;
		case FIXUP_SOURCE_16BIT_SEGMENT:
			FIXME( "16-bit segment relocation at %p, ignoring\n", sourcePtr );
			break;
		case FIXUP_SOURCE_16BIT_POINTER:
			FIXME( "16-bit pointer relocation at %p, ignoring\n", sourcePtr );
			break;
		case FIXUP_SOURCE_16BIT_OFFSET:
			*( (uint16_t *) sourcePtr ) = targetOffset & 0xFFFF;
			break;
		case FIXUP_SOURCE_32BIT_FARPTR:
			*( (uint16_t *) sourcePtr ) = mCodeSel;
			sourcePtr += 2;
			// fall through
		case FIXUP_SOURCE_32BIT_OFFSET:
			*( (uint32_t *) sourcePtr ) = targetOffset;
			break;
		case FIXUP_SOURCE_32BIT_REL:
			FIXME( "32-bit relative relocation at %p, ignoring\n", sourcePtr );
			break;
		default:
			ERR( "unknown relocation type, source 0x%x\n", reloc->sourceType );
			throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}

	return recordSize;
}

uint32_t LEImage::roundToPageSize( uint32_t size, uint32_t pageSize )
{
	return ( ( size / pageSize ) + 1 ) * pageSize;
}
