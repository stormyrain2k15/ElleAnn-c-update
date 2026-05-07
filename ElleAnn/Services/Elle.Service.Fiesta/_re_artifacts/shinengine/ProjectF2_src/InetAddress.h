// InetAddress.h
// CINETADDRESS -- thin wrapper over sockaddr_in.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\inetaddress.h

#pragma once
#include <winsock2.h>
#include "typedef.h"

class CINETADDRESS
{
public:
	CINETADDRESS();
	CINETADDRESS( const char* pszIP, unsigned short usPort );
	CINETADDRESS( DWORD dwIP_NetOrder, unsigned short usPort_HostOrder );
	explicit CINETADDRESS( const sockaddr_in& sa );

	void	SetIP( const char* pszIP );
	void	SetIP( DWORD dwIP_NetOrder );
	void	SetPort( unsigned short usPort );	// host byte order

	DWORD			GetIP()   const;			// network byte order
	unsigned short	GetPort() const;			// host byte order
	const char*		GetIPStr() const;			// dotted decimal

	const sockaddr_in&	GetSockAddr() const { return m_sa; }
	sockaddr_in*		GetSockAddrPtr()    { return &m_sa; }

	bool	IsValid() const;
	void	Clear();

	// true if address is LAN (192.168.x.x) -- cipher inactive on LAN
	bool	IsLAN() const;

private:
	sockaddr_in	m_sa;
	mutable char m_szIP[16];
};
