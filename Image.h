
#ifndef __DOS32_IMAGE_H__
#define __DOS32_IMAGE_H__

#include <stdint.h>
#include <string>
#include <list>


class Image
{
	public:
		Image( const std::string &fileName, uint32_t maxHeapSize ) :
			mFileName( fileName ), mMaxHeapSize( maxHeapSize ) {}
		virtual ~Image() {}

		virtual const std::string &getFileName() const
		{
			return mFileName;
		}

		virtual void load() = 0;
		virtual void *getEntryPoint() const = 0;
		virtual void *getStackPointer() const = 0;
		virtual void *getHeapEnd() const = 0;

	protected:
		std::string mFileName;
		uint32_t mMaxHeapSize;
};


typedef Image *(ImageCreator)( const std::string &fileName, uint32_t maxHeapSize );

class ImageFactory
{
	public:
		ImageFactory( ImageCreator *func );

		static Image *create( const std::string &fileName, uint32_t maxHeapSize );

	private:
		static std::list<ImageCreator *> mCreators;
};


#endif  // __DOS32_IMAGE_H__
