// Socket_IOCP.h
// CSocketIOCP -- per-connection state for an IOCP-based TCP socket.
// Owns the recv/send buffers and overlapped structs.
// All I/O is initiated here; completions dispatch through CIOCPProcess.
//
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\socket_iocp.h

#pragma once
#include <winsock2.h>
#include "typedef.h"
#include "IOBuffer.h"
#include "InetAddress.h"

#define SOCKET_IOCP_RECV_SIZE	( 1024 * 8 )

//------------------------------------------------------------------
enum eSessionState
{
	SESSION_STATE_NONE = 0,
	SESSION_STATE_CONNECTED,
	SESSION_STATE_DISCONNECTING,
	SESSION_STATE_DISCONNECTED,
};

//------------------------------------------------------------------
class CSocketIOCP
{
public:
	CSocketIOCP();
	virtual ~CSocketIOCP();

	// Initialise session with accepted socket + remote address.
	virtual void	Init( SOCKET hSock, const CINETADDRESS& remoteAddr );

	// Tear down -- cancel pending I/O, close socket.
	virtual void	Close();

	// Post a WSARecv to keep the recv pipeline running.
	bool	PostRecv();

	// Copy pData into send buffer and post WSASend if idle.
	bool	Send( const void* pData, int nLen );

	// Called by CIOCPProcess when recv bytes land.
	void	OnRecv( DWORD dwBytes );

	// Called by CIOCPProcess when send completes.
	void	OnSend( DWORD dwBytes );

	// Subclass implements: called once a full packet is buffered.
	virtual void	OnPacket( WORD wOpCode, const void* pData, int nLen ) = 0;

	// Called when the connection drops.
	virtual void	OnDisconnect() = 0;

	SOCKET				GetSocket()      const { return m_hSock; }
	const CINETADDRESS&	GetRemoteAddr()  const { return m_remoteAddr; }
	eSessionState		GetState()       const { return m_eState; }
	bool				IsConnected()    const
	{ return m_eState == SESSION_STATE_CONNECTED; }

protected:
	// Attempt to parse complete packets out of recv buffer.
	void	ProcessRecvBuffer();

	SOCKET			m_hSock;
	CINETADDRESS	m_remoteAddr;
	eSessionState	m_eState;
	CSocketBuffer	m_buf;
	bool			m_bSending;		// send op in flight
	CRITICAL_SECTION m_csRecv;
	CRITICAL_SECTION m_csSend;
};
