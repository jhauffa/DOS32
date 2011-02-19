
#ifndef __DOS32_DEBUG_H__
#define __DOS32_DEBUG_H__

#include <stdio.h>
#include <assert.h>

#define __MSG__(format, ...)	fprintf( stderr, format, ## __VA_ARGS__ )

#define ERR(format, ...)		__MSG__( "ERROR: " format, ## __VA_ARGS__ )

#ifndef NDEBUG
#  define FIXME(format, ...)	__MSG__( "FIXME: " format, ## __VA_ARGS__ )
#  if DEBUG_LEVEL > 1
#    define TRACE(format, ...)	__MSG__( format, ## __VA_ARGS__ )
#  else
#    define TRACE
#  endif
#else
#  define FIXME
#  define TRACE
#endif

#endif  // __DOS32_DEBUG_H__
