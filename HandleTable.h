
#ifndef __DOS32_HANDLE_TABLE_H__
#define __DOS32_HANDLE_TABLE_H__

#include <cstdint>

#include "DOSException.h"


template<class T> class HandleTable
{
	public:
		HandleTable( uint16_t maxEntries ) : mNumEntries( 0 ), mMaxEntries( maxEntries )
		{
			mObjects = new T*[maxEntries];
		}

		virtual ~HandleTable()
		{
			delete[] mObjects;
		}

		uint16_t allocate( T *object )
		{
			if ( mNumEntries == mMaxEntries )
				throw DOSException( DOSException::ERROR_OUT_OF_HANDLES );

			uint16_t handle = mNumEntries;
			while ( mObjects[handle] != NULL )
			{
				if ( ++handle == mMaxEntries )
					handle = 0;
			}
			mObjects[handle] = object;
			mNumEntries++;
			return handle;
		}

		T* release( uint16_t handle )
		{
			T *object = get( handle );
			mObjects[handle] = NULL;
			mNumEntries--;
			return object;
		}

		T* get( uint16_t handle )
		{
			if ( handle >= mMaxEntries )
				throw DOSException( DOSException::ERROR_INVALID_HANDLE );
			T *object = mObjects[handle];
			if ( !object )
				throw DOSException( DOSException::ERROR_INVALID_HANDLE );
			return object;
		}

	private:
		uint16_t mNumEntries, mMaxEntries;
		T** mObjects;
};


#endif  // __DOS32_HANDLE_TABLE_H__
