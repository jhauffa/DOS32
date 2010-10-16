
#include "ImageLoaderException.h"
#include "Image.h"


std::list<ImageCreator *> ImageFactory::mCreators;

		
ImageFactory::ImageFactory( ImageCreator *func )
{
	mCreators.push_back( func );
}

Image *ImageFactory::create( const std::string &fileName, uint32_t maxHeapSize )
{
	// try to find an ImageCreator that can handle the file format
	Image *image = NULL;
	for ( std::list<ImageCreator *>::iterator it = mCreators.begin();
	      it != mCreators.end(); it++ )
	{
		image = (*(*it))( fileName, maxHeapSize );
		try
		{
			image->load();
		}
		catch ( const ImageLoaderException &ex )
		{
			delete image;
			image = NULL;
			if ( ex.getError() != ImageLoaderException::IMG_UNKNOWN_FORMAT )
				throw;
		}
	}

	if ( !image )
		throw ImageLoaderException( ImageLoaderException::IMG_UNKNOWN_FORMAT );
	return image;
}
