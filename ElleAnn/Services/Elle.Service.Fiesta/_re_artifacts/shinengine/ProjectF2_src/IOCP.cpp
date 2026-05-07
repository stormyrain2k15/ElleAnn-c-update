// IOCP.cpp
// CShineIOCP + CIOCPProcess implementation.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\iocp\iocp.cpp
//         E:\ProjectF2\Server\1SharedCode\win2000\iocp\iocp_process.cpp

#include "stdafx.h"
#include "IOCP.h"
#include "Socket_IOCP.h"
#include "ShineAssert.h"

//==================================================================
// CShineIOCP
//==================================================================

CShineIOCP::CShineIOCP()
	: m_hIOCP(NULL), m_nThreads(0), m_bRunning(false)
{
	ZeroMemory( m_hThreads, sizeof(m_hThreads) );
}

CShineIOCP::~CShineIOCP()
{
	Shutdown();
}

bool CShineIOCP::Init( int nWorkerThreads )
{
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE,
	                                  NULL, 0, nWorkerThreads );
	if( !m_hIOCP )
	{
		ShineAssert( false, "CShineIOCP::Init -- CreateIoCompletionPort failed %u",
		             GetLastError() );
		return false;
	}

	m_bRunning = true;
	m_nThreads = nWorkerThreads;

	for( int i = 0; i < nWorkerThreads; i++ )
	{
		DWORD dwID = 0;
		m_hThreads[i] = CreateThread( NULL, 0,
		                              WorkerThreadProc,
		                              this, 0, &dwID );
		if( !m_hThreads[i] )
		{
			ShineAssert( false, "CShineIOCP::Init -- CreateThread failed %u",
			             GetLastError() );
			m_nThreads = i;
			Shutdown();
			return false;
		}
	}

	return true;
}

bool CShineIOCP::Associate( SOCKET hSock, ULONG_PTR pKey )
{
	HANDLE h = CreateIoCompletionPort( (HANDLE)hSock,
	                                   m_hIOCP, pKey, 0 );
	return ( h == m_hIOCP );
}

bool CShineIOCP::PostStatus( ULONG_PTR pKey, DWORD dwBytes,
                              OVERLAPPED* pOL )
{
	return ( PostQueuedCompletionStatus( m_hIOCP, dwBytes,
	                                     pKey, pOL ) != FALSE );
}

void CShineIOCP::Shutdown()
{
	if( !m_bRunning ) return;
	m_bRunning = false;

	// Wake all workers with NULL key so they exit
	for( int i = 0; i < m_nThreads; i++ )
		PostStatus( 0, 0, NULL );

	if( m_nThreads > 0 )
		WaitForMultipleObjects( m_nThreads, m_hThreads, TRUE, 5000 );

	for( int i = 0; i < m_nThreads; i++ )
	{
		if( m_hThreads[i] )
		{
			CloseHandle( m_hThreads[i] );
			m_hThreads[i] = NULL;
		}
	}

	if( m_hIOCP )
	{
		CloseHandle( m_hIOCP );
		m_hIOCP = NULL;
	}

	m_nThreads = 0;
}

DWORD WINAPI CShineIOCP::WorkerThreadProc( LPVOID pParam )
{
	CShineIOCP* pThis = (CShineIOCP*)pParam;
	pThis->WorkerThread();
	return 0;
}

void CShineIOCP::WorkerThread()
{
	DWORD      dwBytes = 0;
	ULONG_PTR  pKey    = 0;
	OVERLAPPED* pOL    = NULL;

	while( m_bRunning )
	{
		BOOL bOK = GetQueuedCompletionStatus(
		    m_hIOCP, &dwBytes, &pKey, &pOL, INFINITE );

		if( !m_bRunning ) break;

		if( pKey == 0 && pOL == NULL )
			break;	// shutdown signal

		OnComplete( pKey, bOK ? dwBytes : 0, pOL );
	}
}

//==================================================================
// CIOCPProcess
//==================================================================

CIOCPProcess::CIOCPProcess()
{
}

CIOCPProcess::~CIOCPProcess()
{
}

void CIOCPProcess::OnComplete( ULONG_PTR pKey, DWORD dwBytes,
                                OVERLAPPED* pOL )
{
	if( !pKey || !pOL ) return;

	CSocketIOCP*  pSession = (CSocketIOCP*)pKey;
	OVERLAPPEDEX* pOLEx    = (OVERLAPPEDEX*)pOL;

	switch( pOLEx->eOp )
	{
	case IO_OP_RECV:
		if( dwBytes == 0 )
			OnDisconnect( pSession );
		else
			OnRecvComplete( pSession, dwBytes );
		break;

	case IO_OP_SEND:
		OnSendComplete( pSession, dwBytes );
		break;

	case IO_OP_ACCEPT:
		OnAcceptComplete( pSession );
		break;

	case IO_OP_DISCONNECT:
		OnDisconnect( pSession );
		break;

	default:
		break;
	}
}

void CIOCPProcess::OnRecvComplete( CSocketIOCP* pSession,
                                    DWORD dwBytes )
{
	if( !pSession ) return;
	pSession->OnRecv( dwBytes );
}

void CIOCPProcess::OnSendComplete( CSocketIOCP* pSession,
                                    DWORD dwBytes )
{
	if( !pSession ) return;
	pSession->OnSend( dwBytes );
}

void CIOCPProcess::OnAcceptComplete( CSocketIOCP* pSession )
{
	/* TODO -- socket acceptor wires this in socket_acceptor.cpp */
}

void CIOCPProcess::OnDisconnect( CSocketIOCP* pSession )
{
	if( !pSession ) return;
	pSession->OnDisconnect();
}
