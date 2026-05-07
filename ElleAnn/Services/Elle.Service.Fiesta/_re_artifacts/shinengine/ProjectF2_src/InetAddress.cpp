// InetAddress.cpp
// CINETADDRESS implementation.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\inetaddress.cpp

#include "stdafx.h"
#include "InetAddress.h"
#include <stdio.h>
#include <string.h>

CINETADDRESS::CINETADDRESS()
{
	ZeroMemory( &m_sa, sizeof(m_sa) );
	ZeroMemory( m_szIP, sizeof(m_szIP) );
	m_sa.sin_family = AF_INET;
}

CINETADDRESS::CINETADDRESS( const char* pszIP, unsigned short usPort )
{
	ZeroMemory( &m_sa, sizeof(m_sa) );
	ZeroMemory( m_szIP, sizeof(m_szIP) );
	m_sa.sin_family = AF_INET;
	SetIP( pszIP );
	SetPort( usPort );
}

CINETADDRESS::CINETADDRESS( DWORD dwIP_NetOrder, unsigned short usPort_HostOrder )
{
	ZeroMemory( &m_sa, sizeof(m_sa) );
	ZeroMemory( m_szIP, sizeof(m_szIP) );
	m_sa.sin_family      = AF_INET;
	m_sa.sin_addr.s_addr = dwIP_NetOrder;
	m_sa.sin_port        = htons( usPort_HostOrder );
}

CINETADDRESS::CINETADDRESS( const sockaddr_in& sa )
{
	ZeroMemory( m_szIP, sizeof(m_szIP) );
	memcpy( &m_sa, &sa, sizeof(m_sa) );
}

void CINETADDRESS::SetIP( const char* pszIP )
{
	if( !pszIP ) return;
	m_sa.sin_addr.s_addr = inet_addr( pszIP );
}

void CINETADDRESS::SetIP( DWORD dwIP_NetOrder )
{
	m_sa.sin_addr.s_addr = dwIP_NetOrder;
}

void CINETADDRESS::SetPort( unsigned short usPort )
{
	m_sa.sin_port = htons( usPort );
}

DWORD CINETADDRESS::GetIP() const
{
	return m_sa.sin_addr.s_addr;
}

unsigned short CINETADDRESS::GetPort() const
{
	return ntohs( m_sa.sin_port );
}

const char* CINETADDRESS::GetIPStr() const
{
	strncpy_s( m_szIP, sizeof(m_szIP),
	           inet_ntoa( m_sa.sin_addr ), _TRUNCATE );
	return m_szIP;
}

bool CINETADDRESS::IsValid() const
{
	return ( m_sa.sin_addr.s_addr != 0 &&
	         m_sa.sin_addr.s_addr != INADDR_NONE );
}

void CINETADDRESS::Clear()
{
	ZeroMemory( &m_sa, sizeof(m_sa) );
	m_sa.sin_family = AF_INET;
}

bool CINETADDRESS::IsLAN() const
{
	// 192.168.x.x in host byte order = 0xC0A8????
	// Cipher is inactive on LAN connections.
	DWORD dwHost = ntohl( m_sa.sin_addr.s_addr );
	return ( (dwHost >> 16) == 0xC0A8 );
}
