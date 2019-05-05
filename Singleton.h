
#ifndef __DOS32_SINGLETON_H__
#define __DOS32_SINGLETON_H__

#include <cassert>


#define DEFINE_INSTANCE( c )  template<> c *Singleton<c>::mInstance
#define PLACE_INSTANCE( c )  template<> c *Singleton<c>::mInstance = NULL


template<class T> class Singleton
{
	public:
		static T &getInstance()
		{
			return *mInstance;
		}

	private:
		static T *mInstance;

		Singleton( const Singleton & );

	protected:
		Singleton( T *inst )
		{
			assert( mInstance == NULL );
			mInstance = inst;
		}

};


#endif  // __DOS32_SINGLETON_H__
