
#ifndef __DOS32_DEBUG_H__
#define __DOS32_DEBUG_H__

#include <assert.h>

#ifndef NDEBUG
#  include <stdio.h>
#  define TRACE(format, ...)	fprintf( stderr, format, ## __VA_ARGS__ )
#else
#  define TRACE
#endif


#endif  // __DOS32_DEBUG_H__
