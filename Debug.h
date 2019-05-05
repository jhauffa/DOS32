
#ifndef __DOS32_DEBUG_H__
#define __DOS32_DEBUG_H__

#include <cstdio>
#include <cassert>

#define __MSG__(format, ...)	fprintf( stderr, format, ## __VA_ARGS__ )

#define ERR(format, ...)		__MSG__( "ERROR: " format, ## __VA_ARGS__ )

#ifndef NDEBUG
#  define FIXME(format, ...)	__MSG__( "FIXME: " format, ## __VA_ARGS__ )
#  if DEBUG_LEVEL > 1
#    define TRACE(format, ...)	__MSG__( format, ## __VA_ARGS__ )
#  else
#    define TRACE(format, ...)
#  endif
#else
#  define FIXME(format, ...)
#  define TRACE(format, ...)
#endif

#endif  // __DOS32_DEBUG_H__
