// IOBuffer.h
// IOBuffer -- circular I/O buffer for IOCP overlapped operations.
// One send buffer + one recv buffer per socket session.
//
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\iobuffer.h

#pragma once
#include <winsock2.h>
#include "typedef.h"

#define IOBUFFER_SIZE		( 1024 * 64 )	// 64KB per direction
#define IOBUFFER_MASK		( IOBUFFER_SIZE - 1 )

//------------------------------------------------------------------
// OVERLAPPEDEX
// Extended OVERLAPPED structure tagged with operation type.
// Passed to WSASend / WSARecv and recovered via GetQueuedCompletionStatus.
//------------------------------------------------------------------
enum eIOOP
{
	IO_OP_RECV = 0,
	IO_OP_SEND,
	IO_OP_ACCEPT,
	IO_OP_DISCONNECT,
};

struct OVERLAPPEDEX
{
	OVERLAPPED	ol;			// must be first
	eIOOP		eOp;
	WSABUF		wsaBuf;
};

//------------------------------------------------------------------
// CIOBuffer
// Lock-free-ish ring buffer.
// Single producer / single consumer per direction assumed.
//------------------------------------------------------------------
class CIOBuffer
{
public:
	CIOBuffer();
	~CIOBuffer();

	void	Init();
	void	Clear();

	// Write raw bytes into buffer. Returns false if overflow.
	bool	Write( const void* pData, int nLen );

	// Peek at the front without consuming. Returns bytes available.
	int		Peek( void* pDest, int nLen );

	// Consume nLen bytes from front.
	void	Consume( int nLen );

	// Total bytes currently in buffer.
	int		GetDataSize() const;

	// Free space remaining.
	int		GetFreeSize() const;

	bool	IsEmpty() const { return m_nHead == m_nTail; }

private:
	BYTE	m_buf[ IOBUFFER_SIZE ];
	int		m_nHead;	// read position
	int		m_nTail;	// write position
	CRITICAL_SECTION m_cs;
};

//------------------------------------------------------------------
// CSocketBuffer
// Pairs send + recv IOBuffer with per-socket OVERLAPPED structs.
//------------------------------------------------------------------
class CSocketBuffer
{
public:
	CSocketBuffer();
	~CSocketBuffer();

	void	Init();
	void	Clear();

	CIOBuffer	m_recvBuf;
	CIOBuffer	m_sendBuf;
	OVERLAPPEDEX m_recvOL;
	OVERLAPPEDEX m_sendOL;
};
