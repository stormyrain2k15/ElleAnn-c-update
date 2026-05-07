// ShineAssert.cpp
// Source: E:\ProjectF2\Server\1SharedCode\shineassert.cpp

#include "stdafx.h"
#include "ShineAssert.h"
#include <windows.h>
#include <time.h>

void ShineAssertFail( const char* pszFile, int nLine,
                      const char* pszFmt, ... )
{
	char szMsg[2048];
	va_list args;
	va_start( args, pszFmt );
	vsnprintf_s( szMsg, sizeof(szMsg), _TRUNCATE, pszFmt, args );
	va_end( args );

	// Timestamp
	time_t t = time(NULL);
	struct tm tm_info;
	localtime_s( &tm_info, &t );
	char szTime[32];
	strftime( szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &tm_info );

	if( pszFile )
	{
		// Debug assert path -- write to debug log + output window
		char szFull[4096];
		_snprintf_s( szFull, sizeof(szFull), _TRUNCATE,
		             "[%s] ASSERT %s(%d): %s\n",
		             szTime, pszFile, nLine, szMsg );
		OutputDebugStringA( szFull );

		FILE* fp = NULL;
		fopen_s( &fp, "assert.log", "a" );
		if( fp ) { fputs( szFull, fp ); fclose( fp ); }
	}
	else
	{
		// ShineLog path -- write to shine.log
		char szFull[4096];
		_snprintf_s( szFull, sizeof(szFull), _TRUNCATE,
		             "[%s] %s\n", szTime, szMsg );
		OutputDebugStringA( szFull );

		FILE* fp = NULL;
		fopen_s( &fp, "shine.log", "a" );
		if( fp ) { fputs( szFull, fp ); fclose( fp ); }
	}
}
