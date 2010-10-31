
#include "os/OS.h"
#include "ExecutionEnvironment.h"
#include "DescriptorTable.h"
#include "ImageLoaderException.h"
#include "Debug.h"
#include "LeImage.h"


namespace
{

// register with ImageFactory

static Image *createLeImage( const std::string &fileName, uint32_t maxHeapSize )
{
	return new LeImage( fileName, maxHeapSize );
}

static ImageFactory creator( createLeImage );

}


LeImage::LeImage( const std::string &fileName, uint32_t maxHeapSize ) :
	Image( fileName, maxHeapSize ), mFile( NULL ), mEntryPoint( NULL ),
	mStackPointer( NULL ), mHeapEnd( NULL )
{
	ExecutionEnvironment &env = ExecutionEnvironment::getInstance();
	const DescriptorTable &descTable = env.getDescriptorTable();
	mCodeSel = descTable.getOsCodeSel();
}

LeImage::~LeImage()
{
	for ( std::vector<MemMap *>::iterator it = mObjectMappings.begin();
		  it != mObjectMappings.end(); ++it )
		delete *it;
	delete mFile;
}

void LeImage::load()
{
	TRACE( "LeImage: loading \"%s\"\n", mFileName.c_str() );
	MemMap *mem = NULL;
	try
	{
		mFile = OS::createFile( mFileName, File::ACC_READ );
		mem = OS::createMemMap( *mFile, MemMap::ACC_READ | MemMap::ACC_WRITE );
	}
	catch ( const OsException &ex )
	{
		delete mem;
		throw ImageLoaderException( ex );
	}

	LeHeader *header = findLeHeader( *mem );
	if ( !header )
		throw ImageLoaderException( ImageLoaderException::IMG_UNKNOWN_FORMAT );
	verifyHeader( header );

	// locate object table, page table, fixup table and fixup record table
	// TODO: move into helper function
	uint8_t *headerPtr = (uint8_t *) header;
	const LePageTableEntry *pageTable = NULL;
	if ( header->pageTableOffset != 0 )
		pageTable = (LePageTableEntry *) ( headerPtr + header->pageTableOffset );
	if ( !pageTable || !mem->isInRange( (void *) pageTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LeObjectTableEntry *objectTable = NULL;
	if ( ( header->objectTableOffset != 0 ) && ( header->numObjects > 0 ) )
		objectTable = (LeObjectTableEntry *) ( headerPtr + header->objectTableOffset );
	if ( !objectTable || !mem->isInRange( (void *) &objectTable[header->numObjects] ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LeFixupTableEntry *fixupTable = NULL;
	if ( header->fixupPageTableOffset != 0 )
		fixupTable = (LeFixupTableEntry *) ( headerPtr + header->fixupPageTableOffset );
	if ( !fixupTable || !mem->isInRange( (void *) fixupTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	const LeFixupRecord *fixupRecordTable = NULL;
	if ( header->fixupRecordTableOffset != 0 )
		fixupRecordTable = (LeFixupRecord *) (headerPtr + header->fixupRecordTableOffset);
	if ( !fixupRecordTable || !mem->isInRange( (void *) fixupRecordTable ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	// load objects into memory
	try
	{
	        // TODO: iterate over numObjects-1, handle stack separately
		for (int i = 0; i < header->numObjects; i++)
		{
			bool isStack = ( i == ( header->espObjectIdx - 1 ) );
			mapObject( *mem, header, &objectTable[i], pageTable, isStack );
		}

		relocate( *mem, header, objectTable, fixupTable, fixupRecordTable );
	}
	catch ( const OsException &ex )
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

void *LeImage::getEntryPoint() const
{
	return mEntryPoint;
}

void *LeImage::getStackPointer() const
{
	return mStackPointer;
}

void *LeImage::getHeapEnd() const
{
	return mHeapEnd;
}

LeHeader *LeImage::findLeHeader( const MemMap &mem ) const
{
	uint16_t *ptr = (uint16_t *) mem.getPtr();
	while ( mem.isInRange( ptr + ( DOS_EXT_TYPE / 2 ) + 1 ) )
	{
		if ( ( ptr[0] == MAGIC_MZ ) && ( ptr[DOS_EXT_TYPE / 2] == DOS_EXT_TYPE_MAGIC ) )
		{
			uint32_t headerOffset = *( (uint32_t *) ( (uint8_t *) ptr + DOS_LE_OFFSET ) );
			TRACE( "LE header offset = 0x%x\n", headerOffset );

			LeHeader *header = (LeHeader *) ( (uint8_t *) ptr + headerOffset );
			if ( mem.isInRange( (void *) ( header+1 ) ) && ( header->magic == MAGIC_LE ) )
			{
				TRACE( "found valid LE header\n" );
				header->dataPagesOffset += (uint32_t) ptr - (uint32_t) mem.getPtr();
				return header;
			}
			else
				TRACE( "found invalid LE header\n" );
		}
		ptr++;
	}
	return NULL;
}

void LeImage::verifyHeader( const LeHeader *header ) const
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

void LeImage::mapObject( const MemMap &mem, const LeHeader *header,
	const LeObjectTableEntry *object, const LePageTableEntry *pageTable, bool isStack )
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
		TRACE( "WARNING: 16-bit code object\n" );

	if ( !mem.isInRange( (void *) &pageTable[object->pageTableIdx +
			object->numPageTableEntries] ) )
		throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

	// Allocate a VM area; ignore permission flags so that code can be
	// patched without having to modify and restore the read-only state.
	// TODO: evaluate cost/benefit/feasability of correct page protection
	MemSize allocSize = roundToPageSize( object->virtualSize, header->pageSize );
	if ( isStack )
		allocSize += mMaxHeapSize;
	MemMap *objectMapping = OS::createMemMap( allocSize,
		MemMap::ACC_READ | MemMap::ACC_WRITE | MemMap::ACC_EXEC );
	TRACE( "new object base = %p\n", objectMapping->getPtr() );
	mObjectMappings.push_back( objectMapping );

	// find and map contiguous regions
	uint32_t startOffset = 0;
	int startIdx = 0;
	uint32_t endOffset = 0;
	for ( int i = 0; i < object->numPageTableEntries; i++ )
	{
		const LePageTableEntry *page = &pageTable[object->pageTableIdx + i - 1];
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

void LeImage::relocate( const MemMap &mem, const LeHeader *header,
	const LeObjectTableEntry *objectTable, const LeFixupTableEntry *fixupTable,
	const LeFixupRecord *fixupRecordTable )
{
	for ( int i = 0; i < header->numObjects; i++ )
	{
		int firstPage = objectTable[i].pageTableIdx - 1;
		if ( !mem.isInRange(
				(void *) &fixupTable[firstPage + objectTable[i].numPageTableEntries] ) )
			throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

		for ( int j = 0; j < objectTable[i].numPageTableEntries; j++ )
		{
			const LeFixupRecord *relocStart = (const LeFixupRecord *)
				( (uint8_t *) fixupRecordTable + fixupTable[firstPage + j] );
			const LeFixupRecord *relocEnd =	(const LeFixupRecord *)
				( (uint8_t *) fixupRecordTable + fixupTable[firstPage + j + 1] );
			if ( !mem.isInRange( (void *) relocEnd ) )
				throw ImageLoaderException( ImageLoaderException::IMG_CORRUPTED );

			while ( relocStart < relocEnd )
			{
				int size = processRelocationRecord( i, j * header->pageSize, relocStart );
				relocStart = (const LeFixupRecord *) ( (uint8_t *) relocStart + size );
			}
		}
	}
}

int LeImage::processRelocationRecord( int objectIdx, uint32_t objectOffset,
	const LeFixupRecord *reloc )
{
	if ( reloc->sourceType & FIXUP_SOURCE_FLAGS_LIST )
	{
		TRACE( "relocation source list\n" );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}
	if ( ( reloc->targetType & FIXUP_TARGET_TYPE_MASK ) != FIXUP_TARGET_INTERNAL )
	{
		TRACE( "unknown relocation type, target 0x%x\n", reloc->targetType );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}
	if ( ( reloc->targetType & FIXUP_TARGET_FLAGS_ADD ) != 0 )
	{
		TRACE( "additive relocation\n" );
		throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}

	int16_t pageOffset = *( (int16_t *) &reloc->data[0] );
	uint8_t *sourcePtr = (uint8_t *) mObjectMappings[objectIdx]->getPtr() +	objectOffset +
		pageOffset;
	uint8_t targetObjectIdx = reloc->data[2];
//	TRACE( "relocation at %p: target object %u, ", sourcePtr, targetObjectIdx );
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
//	TRACE( "target offset = 0x%x\n", targetOffset );

	switch ( reloc->sourceType & FIXUP_SOURCE_TYPE_MASK )
	{
		case FIXUP_SOURCE_BYTE:
			TRACE( "TODO: byte relocation at %p, ignoring\n", sourcePtr );
			break;
		case FIXUP_SOURCE_16BIT_SEGMENT:
			TRACE( "TODO: 16-bit segment relocation at %p, ignoring\n", sourcePtr );
			break;
		case FIXUP_SOURCE_16BIT_POINTER:
			TRACE( "TODO: 16-bit pointer relocation at %p, ignoring\n", sourcePtr );
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
			TRACE( "TODO: 32-bit relative relocation at %p, ignoring\n", sourcePtr );
			break;
		default:
			TRACE( "unknown relocation type, source 0x%x\n", reloc->sourceType );
			throw ImageLoaderException( ImageLoaderException::IMG_NOT_SUPPORTED );
	}

	return recordSize;
}

uint32_t LeImage::roundToPageSize( uint32_t size, uint32_t pageSize )
{
	return ( ( size / pageSize ) + 1 ) * pageSize;
}
