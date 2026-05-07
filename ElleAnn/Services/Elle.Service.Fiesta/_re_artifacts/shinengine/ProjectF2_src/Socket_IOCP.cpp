// Socket_IOCP.cpp
// CSocketIOCP implementation.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\socket_iocp.cpp

#include "stdafx.h"
#include "Socket_IOCP.h"
#include "ShineAssert.h"
#include "GPacket.h"

CSocketIOCP::CSocketIOCP()
	: m_hSock(INVALID_SOCKET),
	  m_eState(SESSION_STATE_NONE),
	  m_bSending(false)
{
	InitializeCriticalSection( &m_csRecv );
	InitializeCriticalSection( &m_csSend );
}

CSocketIOCP::~CSocketIOCP()
{
	Close();
	DeleteCriticalSection( &m_csRecv );
	DeleteCriticalSection( &m_csSend );
}

void CSocketIOCP::Init( SOCKET hSock, const CINETADDRESS& remoteAddr )
{
	m_hSock      = hSock;
	m_remoteAddr = remoteAddr;
	m_eState     = SESSION_STATE_CONNECTED;
	m_bSending   = false;
	m_buf.Init();
}

void CSocketIOCP::Close()
{
	if( m_hSock == INVALID_SOCKET ) return;
	m_eState = SESSION_STATE_DISCONNECTING;
	shutdown( m_hSock, SD_BOTH );
	closesocket( m_hSock );
	m_hSock  = INVALID_SOCKET;
	m_eState = SESSION_STATE_DISCONNECTED;
}

bool CSocketIOCP::PostRecv()
{
	if( m_hSock == INVALID_SOCKET ) return false;

	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	m_buf.m_recvOL.wsaBuf.buf = (char*)m_buf.m_recvBuf.GetData();
	m_buf.m_recvOL.wsaBuf.len = SOCKET_IOCP_RECV_SIZE;
	ZeroMemory( &m_buf.m_recvOL.ol, sizeof(OVERLAPPED) );

	int nRet = WSARecv( m_hSock,
	                    &m_buf.m_recvOL.wsaBuf, 1,
	                    &dwBytes, &dwFlags,
	                    &m_buf.m_recvOL.ol, NULL );

	if( nRet == SOCKET_ERROR )
	{
		DWORD dwErr = WSAGetLastError();
		if( dwErr != WSA_IO_PENDING )
		{
			ShineLog( "CSocketIOCP::PostRecv WSARecv error %u", dwErr );
			return false;
		}
	}

	return true;
}

bool CSocketIOCP::Send( const void* pData, int nLen )
{
	if( m_hSock == INVALID_SOCKET || nLen <= 0 ) return false;

	EnterCriticalSection( &m_csSend );

	bool bOK = m_buf.m_sendBuf.Write( pData, nLen );
	if( !bOK )
	{
		ShineLog( "CSocketIOCP::Send -- send buffer overflow" );
		LeaveCriticalSection( &m_csSend );
		return false;
	}

	if( !m_bSending )
	{
		// Kick off WSASend
		m_bSending = true;
		int nPending = m_buf.m_sendBuf.Peek(
		    (void*)m_buf.m_sendOL.wsaBuf.buf,
		    GPACKET_MAX_LEN );

		m_buf.m_sendOL.wsaBuf.len = nPending;
		ZeroMemory( &m_buf.m_sendOL.ol, sizeof(OVERLAPPED) );

		DWORD dwBytes = 0;
		int nRet = WSASend( m_hSock,
		                    &m_buf.m_sendOL.wsaBuf, 1,
		                    &dwBytes, 0,
		                    &m_buf.m_sendOL.ol, NULL );
		if( nRet == SOCKET_ERROR )
		{
			DWORD dwErr = WSAGetLastError();
			if( dwErr != WSA_IO_PENDING )
			{
				ShineLog( "CSocketIOCP::Send WSASend error %u", dwErr );
				m_bSending = false;
				LeaveCriticalSection( &m_csSend );
				return false;
			}
		}
	}

	LeaveCriticalSection( &m_csSend );
	return true;
}

void CSocketIOCP::OnRecv( DWORD dwBytes )
{
	EnterCriticalSection( &m_csRecv );

	// Bytes already DMA'd into recv buffer by IOCP
	// Advance tail by dwBytes and try to parse complete packets
	m_buf.m_recvBuf.Write(
	    m_buf.m_recvOL.wsaBuf.buf, (int)dwBytes );

	ProcessRecvBuffer();

	LeaveCriticalSection( &m_csRecv );

	// Re-arm recv
	PostRecv();
}

void CSocketIOCP::OnSend( DWORD dwBytes )
{
	EnterCriticalSection( &m_csSend );

	m_buf.m_sendBuf.Consume( (int)dwBytes );

	int nRemain = m_buf.m_sendBuf.GetDataSize();
	if( nRemain > 0 )
	{
		// More data queued -- fire another WSASend
		int nChunk = m_buf.m_sendBuf.Peek(
		    (void*)m_buf.m_sendOL.wsaBuf.buf,
		    GPACKET_MAX_LEN );

		m_buf.m_sendOL.wsaBuf.len = nChunk;
		ZeroMemory( &m_buf.m_sendOL.ol, sizeof(OVERLAPPED) );

		DWORD dwOut = 0;
		WSASend( m_hSock,
		         &m_buf.m_sendOL.wsaBuf, 1,
		         &dwOut, 0,
		         &m_buf.m_sendOL.ol, NULL );
	}
	else
	{
		m_bSending = false;
	}

	LeaveCriticalSection( &m_csSend );
}

void CSocketIOCP::ProcessRecvBuffer()
{
	// Parse complete packets: [wLen(2)][wOpCode(2)][payload(wLen-2)]
	for(;;)
	{
		int nAvail = m_buf.m_recvBuf.GetDataSize();
		if( nAvail < (int)sizeof(PROTO_NC_HEADER) ) break;

		PROTO_NC_HEADER hdr;
		m_buf.m_recvBuf.Peek( &hdr, sizeof(hdr) );

		int nTotal = (int)sizeof(PROTO_NC_HEADER) + (int)hdr.wLen;
		if( nAvail < nTotal ) break;

		// Full packet available
		BYTE pktBuf[ GPACKET_MAX_LEN + sizeof(PROTO_NC_HEADER) ];
		m_buf.m_recvBuf.Peek( pktBuf, nTotal );
		m_buf.m_recvBuf.Consume( nTotal );

		const BYTE* pPayload = pktBuf + sizeof(PROTO_NC_HEADER) + sizeof(WORD);
		int  nPayloadLen     = (int)hdr.wLen - (int)sizeof(WORD);
		if( nPayloadLen < 0 ) nPayloadLen = 0;

		OnPacket( hdr.wOpCode, pPayload, nPayloadLen );
	}
}
