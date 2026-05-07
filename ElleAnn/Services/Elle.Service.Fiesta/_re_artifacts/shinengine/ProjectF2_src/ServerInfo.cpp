// ServerInfo.cpp
// CServerInfo -- parses _ServerInfo.txt (Fiesta server grammar).
// Source: E:\ProjectF2\Server\1SharedCode\serverinfo\serverinfo.cpp
//
// File grammar (confirmed from live _ServerInfo.txt on disk):
//
//   SERVER_INFO "Name"  ID, W, Z, F, "IP", Port, Backlog, MaxConn
//   ODBC_INFO   "Name"  ID, F, "ConnectionString"
//   ; comment line
//   $END
//
// Same grammar read by Zone, WorldManager, Login, OPTool, and Elle.

#include "stdafx.h"
#include "ServerInfo.h"
#include "ShineAssert.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//------------------------------------------------------------------
// CServerInfo
//------------------------------------------------------------------

CServerInfo::CServerInfo()
	: m_nServerCount(0), m_nODBCCount(0)
{
	ZeroMemory( m_servers, sizeof(m_servers) );
	ZeroMemory( m_odbc,    sizeof(m_odbc) );
}

CServerInfo::~CServerInfo()
{
}

bool CServerInfo::Load( const char* pszPath )
{
	FILE* fp = NULL;
	fopen_s( &fp, pszPath, "r" );
	if( !fp )
	{
		ShineAssert( false, "CServerInfo::Load -- file not found: %s", pszPath );
		return false;
	}

	char szLine[1024];
	while( fgets( szLine, sizeof(szLine), fp ) )
	{
		// Strip newline
		char* pCR = strpbrk( szLine, "\r\n" );
		if( pCR ) *pCR = '\0';

		// Skip blank + comment lines
		const char* pTok = szLine;
		while( *pTok == ' ' || *pTok == '\t' ) pTok++;
		if( *pTok == '\0' || *pTok == ';' ) continue;

		if( _strnicmp( pTok, "$END", 4 ) == 0 )
			break;

		if( _strnicmp( pTok, "SERVER_INFO", 11 ) == 0 )
			ParseServerLine( pTok + 11 );
		else if( _strnicmp( pTok, "ODBC_INFO", 9 ) == 0 )
			ParseODBCLine( pTok + 9 );
	}

	fclose( fp );
	return true;
}

//------------------------------------------------------------------
// SERVER_INFO "Name" ID, W, Z, F, "IP", Port, Backlog, MaxConn
//------------------------------------------------------------------
void CServerInfo::ParseServerLine( const char* pszLine )
{
	if( m_nServerCount >= SERVER_INFO_MAX ) return;

	SERVERINFO_ENTRY& e = m_servers[ m_nServerCount ];
	ZeroMemory( &e, sizeof(e) );

	// Name (quoted string)
	const char* p = strchr( pszLine, '"' );
	if( !p ) return;
	p++;
	const char* pEnd = strchr( p, '"' );
	if( !pEnd ) return;
	int nLen = (int)(pEnd - p);
	if( nLen >= (int)sizeof(e.szName) ) nLen = sizeof(e.szName) - 1;
	memcpy( e.szName, p, nLen );
	e.szName[nLen] = '\0';

	// Numeric fields: ID, W, Z, F
	p = pEnd + 1;
	while( *p == ',' || *p == ' ' || *p == '\t' ) p++;
	e.nID = atoi( p );
	p = strchr( p, ',' ); if(!p) return; p++;
	e.nW  = atoi( p );
	p = strchr( p, ',' ); if(!p) return; p++;
	e.nZ  = atoi( p );
	p = strchr( p, ',' ); if(!p) return; p++;
	e.nF  = atoi( p );

	// IP string (quoted)
	p = strchr( p, '"' ); if(!p) return; p++;
	pEnd = strchr( p, '"' ); if(!pEnd) return;
	nLen = (int)(pEnd - p);
	if( nLen >= (int)sizeof(e.szIP) ) nLen = sizeof(e.szIP) - 1;
	memcpy( e.szIP, p, nLen );
	e.szIP[nLen] = '\0';

	// Port, Backlog, MaxConn
	p = pEnd + 1;
	p = strchr( p, ',' ); if(!p) return; p++;
	e.nPort    = atoi( p );
	p = strchr( p, ',' ); if(!p) goto done;
	p++;
	e.nBacklog = atoi( p );
	p = strchr( p, ',' ); if(!p) goto done;
	p++;
	e.nMaxConn = atoi( p );

done:
	m_nServerCount++;
}

//------------------------------------------------------------------
// ODBC_INFO "Name" ID, F, "ConnString"
//------------------------------------------------------------------
void CServerInfo::ParseODBCLine( const char* pszLine )
{
	if( m_nODBCCount >= ODBC_INFO_MAX ) return;

	ODBCINFO_ENTRY& e = m_odbc[ m_nODBCCount ];
	ZeroMemory( &e, sizeof(e) );

	const char* p = strchr( pszLine, '"' );
	if( !p ) return;
	p++;
	const char* pEnd = strchr( p, '"' );
	if( !pEnd ) return;
	int nLen = (int)(pEnd - p);
	if( nLen >= (int)sizeof(e.szName) ) nLen = sizeof(e.szName) - 1;
	memcpy( e.szName, p, nLen );
	e.szName[nLen] = '\0';

	p = pEnd + 1;
	while( *p == ',' || *p == ' ' || *p == '\t' ) p++;
	e.nID = atoi( p );
	p = strchr( p, ',' ); if(!p) return; p++;
	e.nF  = atoi( p );

	// Connection string (quoted)
	p = strchr( p, '"' ); if(!p) return; p++;
	pEnd = strchr( p, '"' ); if(!pEnd) return;
	nLen = (int)(pEnd - p);
	if( nLen >= (int)sizeof(e.szConnStr) ) nLen = sizeof(e.szConnStr) - 1;
	memcpy( e.szConnStr, p, nLen );
	e.szConnStr[nLen] = '\0';

	m_nODBCCount++;
}

//------------------------------------------------------------------
// Lookup helpers
//------------------------------------------------------------------
const SERVERINFO_ENTRY* CServerInfo::FindServer( const char* pszName ) const
{
	for( int i = 0; i < m_nServerCount; i++ )
		if( _stricmp( m_servers[i].szName, pszName ) == 0 )
			return &m_servers[i];
	return NULL;
}

const SERVERINFO_ENTRY* CServerInfo::FindServerByID( int nID ) const
{
	for( int i = 0; i < m_nServerCount; i++ )
		if( m_servers[i].nID == nID )
			return &m_servers[i];
	return NULL;
}

const ODBCINFO_ENTRY* CServerInfo::FindODBC( const char* pszName ) const
{
	for( int i = 0; i < m_nODBCCount; i++ )
		if( _stricmp( m_odbc[i].szName, pszName ) == 0 )
			return &m_odbc[i];
	return NULL;
}
