// IOCP.h
// CShineIOCP -- Windows I/O completion port manager.
// Wraps CreateIoCompletionPort, worker thread pool, and
// per-key completion dispatch.
//
// Source: E:\ProjectF2\Server\1SharedCode\win2000\iocp\iocp.h

#pragma once
#include <windows.h>
#include <winsock2.h>
#include "typedef.h"
#include "IOBuffer.h"

#define IOCP_WORKER_THREADS		4
#define IOCP_MAX_SESSIONS		2000

//------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------
class CSocketIOCP;
class CIOCPProcess;

//------------------------------------------------------------------
// CShineIOCP
// One instance per server process.
// Associates sockets with the completion port and dispatches
// completed I/O operations to CIOCPProcess handlers.
//------------------------------------------------------------------
class CShineIOCP
{
public:
	CShineIOCP();
	virtual ~CShineIOCP();

	// Create completion port + start worker threads.
	bool	Init( int nWorkerThreads = IOCP_WORKER_THREADS );

	// Associate a socket with the completion port.
	// pKey is the per-connection context (usually CSocketIOCP*).
	bool	Associate( SOCKET hSock, ULONG_PTR pKey );

	// Post a user-defined completion packet.
	bool	PostStatus( ULONG_PTR pKey, DWORD dwBytes = 0, OVERLAPPED* pOL = NULL );

	// Graceful shutdown -- drain queue then stop workers.
	void	Shutdown();

	HANDLE	GetHandle() const { return m_hIOCP; }
	bool	IsRunning() const { return m_bRunning; }

protected:
	// Worker thread proc -- override to customise dispatch.
	virtual void	WorkerThread();
	static DWORD WINAPI WorkerThreadProc( LPVOID pParam );

	// Called when an I/O operation completes.
	// Override in CIOCPProcess to dispatch to session handlers.
	virtual void	OnComplete( ULONG_PTR pKey, DWORD dwBytes,
	                            OVERLAPPED* pOL ) = 0;

	HANDLE		m_hIOCP;
	HANDLE		m_hThreads[ IOCP_WORKER_THREADS ];
	int			m_nThreads;
	volatile bool m_bRunning;
};

//------------------------------------------------------------------
// IOCP_Process.h
// CIOCPProcess -- dispatches completed operations to session methods.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\iocp\iocp_process.h
//------------------------------------------------------------------
class CIOCPProcess : public CShineIOCP
{
public:
	CIOCPProcess();
	virtual ~CIOCPProcess();

protected:
	virtual void	OnComplete( ULONG_PTR pKey, DWORD dwBytes,
	                            OVERLAPPED* pOL ) override;

	void	OnRecvComplete( CSocketIOCP* pSession, DWORD dwBytes );
	void	OnSendComplete( CSocketIOCP* pSession, DWORD dwBytes );
	void	OnAcceptComplete( CSocketIOCP* pSession );
	void	OnDisconnect( CSocketIOCP* pSession );
};
