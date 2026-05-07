// ServerInfo.h
// CServerInfo -- parses _ServerInfo.txt (Fiesta server grammar).
// Source: E:\ProjectF2\Server\1SharedCode\serverinfo\serverinfo.h
//
// All ShineEngine server binaries + Elle services read
// the same _ServerInfo.txt via this class.

#pragma once
#include "typedef.h"

#define SERVER_INFO_MAX		64
#define ODBC_INFO_MAX		32
#define SERVERINFO_NAMELEN	64
#define SERVERINFO_IPLEN	32
#define ODBC_CONNSTR_LEN	512

//------------------------------------------------------------------
struct SERVERINFO_ENTRY
{
	char	szName[ SERVERINFO_NAMELEN ];
	int		nID;
	int		nW;
	int		nZ;
	int		nF;
	char	szIP[ SERVERINFO_IPLEN ];
	int		nPort;
	int		nBacklog;
	int		nMaxConn;
};

//------------------------------------------------------------------
struct ODBCINFO_ENTRY
{
	char	szName[ SERVERINFO_NAMELEN ];
	int		nID;
	int		nF;
	char	szConnStr[ ODBC_CONNSTR_LEN ];
};

//------------------------------------------------------------------
class CServerInfo
{
public:
	CServerInfo();
	~CServerInfo();

	bool	Load( const char* pszPath );

	const SERVERINFO_ENTRY*	FindServer( const char* pszName ) const;
	const SERVERINFO_ENTRY*	FindServerByID( int nID ) const;
	const ODBCINFO_ENTRY*	FindODBC( const char* pszName ) const;

	int		GetServerCount() const { return m_nServerCount; }
	int		GetODBCCount()   const { return m_nODBCCount; }

	const SERVERINFO_ENTRY&	GetServer( int i ) const
	{ return m_servers[i]; }
	const ODBCINFO_ENTRY&	GetODBC( int i ) const
	{ return m_odbc[i]; }

private:
	void	ParseServerLine( const char* pszLine );
	void	ParseODBCLine( const char* pszLine );

	SERVERINFO_ENTRY	m_servers[ SERVER_INFO_MAX ];
	ODBCINFO_ENTRY		m_odbc[ ODBC_INFO_MAX ];
	int					m_nServerCount;
	int					m_nODBCCount;
};
