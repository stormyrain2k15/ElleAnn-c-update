// ShineAssert.h
// ShineEngine custom assert + formatted error output.
// Source: E:\ProjectF2\Server\1SharedCode\shineassert.h

#pragma once
#include <stdio.h>
#include <stdarg.h>

void ShineAssertFail( const char* pszFile, int nLine,
                      const char* pszFmt, ... );

#ifdef _DEBUG
  #define ShineAssert( expr, fmt, ... ) \
    do { if(!(expr)) ShineAssertFail(__FILE__,__LINE__,fmt,##__VA_ARGS__); } while(0)
#else
  #define ShineAssert( expr, fmt, ... ) \
    do { (void)(expr); } while(0)
#endif

// Always-on log even in release
#define ShineLog( fmt, ... ) \
  ShineAssertFail( NULL, 0, fmt, ##__VA_ARGS__ )
