// IOBuffer.cpp
// CIOBuffer / CSocketBuffer implementation.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\network\iobuffer.cpp

#include "stdafx.h"
#include "IOBuffer.h"
#include <string.h>

//==================================================================
// CIOBuffer
//==================================================================

CIOBuffer::CIOBuffer()
	: m_nHead(0), m_nTail(0)
{
	ZeroMemory( m_buf, sizeof(m_buf) );
	InitializeCriticalSection( &m_cs );
}

CIOBuffer::~CIOBuffer()
{
	DeleteCriticalSection( &m_cs );
}

void CIOBuffer::Init()
{
	EnterCriticalSection( &m_cs );
	m_nHead = 0;
	m_nTail = 0;
	ZeroMemory( m_buf, sizeof(m_buf) );
	LeaveCriticalSection( &m_cs );
}

void CIOBuffer::Clear()
{
	Init();
}

int CIOBuffer::GetDataSize() const
{
	return ( m_nTail - m_nHead + IOBUFFER_SIZE ) & IOBUFFER_MASK;
}

int CIOBuffer::GetFreeSize() const
{
	return IOBUFFER_SIZE - 1 - GetDataSize();
}

bool CIOBuffer::Write( const void* pData, int nLen )
{
	if( nLen <= 0 ) return true;
	if( GetFreeSize() < nLen ) return false;

	EnterCriticalSection( &m_cs );

	const BYTE* pSrc = (const BYTE*)pData;
	int nTail = m_nTail;

	int nFirstChunk = IOBUFFER_SIZE - nTail;
	if( nFirstChunk >= nLen )
	{
		memcpy( m_buf + nTail, pSrc, nLen );
	}
	else
	{
		memcpy( m_buf + nTail, pSrc,              nFirstChunk );
		memcpy( m_buf,         pSrc + nFirstChunk, nLen - nFirstChunk );
	}

	m_nTail = ( nTail + nLen ) & IOBUFFER_MASK;

	LeaveCriticalSection( &m_cs );
	return true;
}

int CIOBuffer::Peek( void* pDest, int nLen )
{
	int nAvail = GetDataSize();
	if( nLen > nAvail ) nLen = nAvail;
	if( nLen <= 0 )     return 0;

	BYTE* pDst = (BYTE*)pDest;
	int nHead  = m_nHead;

	int nFirstChunk = IOBUFFER_SIZE - nHead;
	if( nFirstChunk >= nLen )
	{
		memcpy( pDst, m_buf + nHead, nLen );
	}
	else
	{
		memcpy( pDst,              m_buf + nHead, nFirstChunk );
		memcpy( pDst + nFirstChunk, m_buf,        nLen - nFirstChunk );
	}

	return nLen;
}

void CIOBuffer::Consume( int nLen )
{
	if( nLen <= 0 ) return;
	EnterCriticalSection( &m_cs );
	m_nHead = ( m_nHead + nLen ) & IOBUFFER_MASK;
	LeaveCriticalSection( &m_cs );
}

//==================================================================
// CSocketBuffer
//==================================================================

CSocketBuffer::CSocketBuffer()
{
	ZeroMemory( &m_recvOL, sizeof(m_recvOL) );
	ZeroMemory( &m_sendOL, sizeof(m_sendOL) );
	m_recvOL.eOp = IO_OP_RECV;
	m_sendOL.eOp = IO_OP_SEND;
}

CSocketBuffer::~CSocketBuffer()
{
}

void CSocketBuffer::Init()
{
	m_recvBuf.Init();
	m_sendBuf.Init();
	ZeroMemory( &m_recvOL, sizeof(m_recvOL) );
	ZeroMemory( &m_sendOL, sizeof(m_sendOL) );
	m_recvOL.eOp = IO_OP_RECV;
	m_sendOL.eOp = IO_OP_SEND;
}

void CSocketBuffer::Clear()
{
	Init();
}
