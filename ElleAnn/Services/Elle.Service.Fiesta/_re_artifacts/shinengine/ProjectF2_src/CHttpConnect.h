// CHttpConnect.h
// Zone server HTTP client -- security token endpoint
//
// Source: all method signatures recovered from 5ZoneServer2.pdb.
// Original source path: d:\Jenkins\workspace\Server - CH - Release\
//                       Server\5ZoneServer2\html\httpconnect.cpp
//
// Used exclusively by LuaScriptSecurity to POST to the external
// security validation endpoint and retrieve an encrypted token.
// Elle's HTTP service (port 8000) is the target of that POST.
//
// Call sequence (from LuaScriptSecurity::cExecCheck):
//   SetServer   -> SetPortA -> SetFuntion -> SetParameter
//   -> MakeAddress -> InitConnection -> QueryToServer -> GetToken

#pragma once
#include <windows.h>
#include <winsock2.h>
#include <string>

//----------------------------------------------------------------
// GET_TOKEN -- result struct populated by GetToken()
//----------------------------------------------------------------
struct GET_TOKEN
{
	char	szToken[512];	// raw encrypted blob from HTTP response
	int		nTokenLen;
	bool	bValid;

	GET_TOKEN() : nTokenLen(0), bValid(false)
	{ ZeroMemory( szToken, sizeof(szToken) ); }
};

//----------------------------------------------------------------
// HTTPString -- HTTP response parser helper
// Source: HTTPString::hs_HTMMRead + HTTPString::hs_Decoding in PDB
//----------------------------------------------------------------
class HTTPString
{
public:
	HTTPString();
	~HTTPString();

	// Read raw HTTP response into internal buffer
	bool	hs_HTMMRead( const char* pszRaw, int nLen );

	// Decode the token field from the parsed response
	bool	hs_Decoding( GET_TOKEN* pOut );

	const char*	GetBody()   const { return m_szBody; }
	int			GetStatus() const { return m_nStatus; }

private:
	char	m_szBody[4096];
	int		m_nStatus;
};

//----------------------------------------------------------------
// CHttpConnect
// Single-use HTTP POST client. One instance per security check.
// Not thread-safe -- Zone creates one per Lua security call.
//----------------------------------------------------------------
class CHttpConnect
{
public:
	CHttpConnect();
	~CHttpConnect();

	// --- Setup (call before MakeAddress) ---

	// Set target host. Stored internally; does NOT resolve yet.
	void	SetServer( const char* pszHost );

	// Set target port (network byte order handled internally).
	void	SetPortA( unsigned short usPort );

	// Set the URL path / function name (e.g. "/security/token").
	// Named SetFuntion in PDB -- matches original typo.
	void	SetFuntion( const char* pszPath );

	// Append a POST key=value parameter.
	// Multiple calls accumulate into application/x-www-form-urlencoded body.
	void	SetParameter( const char* pszKey, const char* pszValue );

	// --- Execution ---

	// Build the full request URL + body from accumulated params.
	// Must be called after all Set* calls and before InitConnection.
	void	MakeAddress();

	// Open TCP socket to host:port. dwTimeout in milliseconds.
	bool	InitConnection( unsigned long dwTimeout );

	// Send the HTTP POST and block until response headers arrive.
	bool	QueryToServer();

	// Parse the response and fill pToken.
	// Returns true if a valid encrypted token was found.
	bool	GetToken( GET_TOKEN* pToken );

	// --- Cleanup ---
	void	Close();

private:
	char			m_szHost[256];
	unsigned short	m_usPort;
	char			m_szPath[256];
	std::string		m_strParams;	// URL-encoded POST body
	std::string		m_strRequest;	// full HTTP request string
	std::string		m_strResponse;	// raw server response
	SOCKET			m_hSocket;
	HTTPString		m_parser;
	bool			m_bConnected;
};

//================================================================
// Inline stubs
// Full implementation lives in httpconnect.cpp (not yet recovered).
// These stubs satisfy the linker until the .cpp is reconstructed.
//================================================================

inline CHttpConnect::CHttpConnect()
	: m_usPort(0), m_hSocket(INVALID_SOCKET), m_bConnected(false)
{
	ZeroMemory( m_szHost, sizeof(m_szHost) );
	ZeroMemory( m_szPath, sizeof(m_szPath) );
}

inline CHttpConnect::~CHttpConnect()
{
	Close();
}

inline void CHttpConnect::SetServer( const char* pszHost )
{
	if( pszHost )
		strncpy_s( m_szHost, sizeof(m_szHost), pszHost, _TRUNCATE );
}

inline void CHttpConnect::SetPortA( unsigned short usPort )
{
	m_usPort = usPort;
}

inline void CHttpConnect::SetFuntion( const char* pszPath )
{
	if( pszPath )
		strncpy_s( m_szPath, sizeof(m_szPath), pszPath, _TRUNCATE );
}

inline void CHttpConnect::SetParameter( const char* pszKey, const char* pszValue )
{
	if( !m_strParams.empty() )
		m_strParams += '&';
	// minimal URL encoding -- full implementation in .cpp
	m_strParams += pszKey;
	m_strParams += '=';
	m_strParams += pszValue;
}

inline void CHttpConnect::MakeAddress()
{
	// Build: POST {path} HTTP/1.0\r\nHost: {host}\r\n
	//        Content-Type: application/x-www-form-urlencoded\r\n
	//        Content-Length: {len}\r\n\r\n{params}
	char szLen[16];
	_itoa_s( (int)m_strParams.size(), szLen, sizeof(szLen), 10 );

	m_strRequest  = "POST ";
	m_strRequest += m_szPath;
	m_strRequest += " HTTP/1.0\r\nHost: ";
	m_strRequest += m_szHost;
	m_strRequest += "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
	m_strRequest += szLen;
	m_strRequest += "\r\n\r\n";
	m_strRequest += m_strParams;
}

inline void CHttpConnect::Close()
{
	if( m_hSocket != INVALID_SOCKET )
	{
		shutdown( m_hSocket, SD_BOTH );
		closesocket( m_hSocket );
		m_hSocket    = INVALID_SOCKET;
		m_bConnected = false;
	}
}
