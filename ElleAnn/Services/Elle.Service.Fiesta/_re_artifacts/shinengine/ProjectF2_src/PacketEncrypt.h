// PacketEncrypt.h
// Packet encryption / decryption.
// Shared between client (CSCode) and all server binaries.
// Source: E:\ProjectF2\CSCode\packetencrypt.h
//
// Algorithm: LCG (linear congruential generator), 16-bit state.
// Seed delivered in opcode 0C03 from Login server as WORD.
// Cipher is NOT active on LAN (192.168.x.x connections).
// All WAN connections (172.20.x.x etc.) use cipher on every byte.
//
// Key constants: TODO -- validate against 172.20 capture.
// Current values are candidate parameters; tune if opcode
// decryption of Port 61483 capture does not produce known values.

#pragma once
#include "typedef.h"

//------------------------------------------------------------------
// LCG parameters
// state_new = (state * A + B) & 0xFFFF
// Key byte  = state_new & 0xFF
//
// NOTE: constants below are candidates derived from similar
// ShineEngine regional builds. Confirm with 172.20 session capture.
//------------------------------------------------------------------
#define PKTENC_LCG_A	0x4E6D
#define PKTENC_LCG_B	0x5249

class CPacketEncrypt
{
public:
	CPacketEncrypt();

	// Initialise with seed from opcode 0C03.
	void	Init( WORD wSeed );
	void	Reset();
	bool	IsInit() const { return m_bInit; }

	// Transform nLen bytes in pBuf in-place.
	// Same function used for both encrypt and decrypt (XOR-symmetric).
	void	Cipher( BYTE* pBuf, int nLen );

	// Transform a single opcode word (applied to header separately).
	WORD	CipherOpCode( WORD wOpCode );

private:
	WORD	Tick();		// advance LCG, return new state

	WORD	m_wState;
	bool	m_bInit;
};

//------------------------------------------------------------------
// Inline implementation
//------------------------------------------------------------------
inline CPacketEncrypt::CPacketEncrypt()
	: m_wState(0), m_bInit(false)
{}

inline void CPacketEncrypt::Reset()
{
	m_wState = 0;
	m_bInit  = false;
}

inline void CPacketEncrypt::Init( WORD wSeed )
{
	m_wState = wSeed;
	m_bInit  = true;
}

inline WORD CPacketEncrypt::Tick()
{
	m_wState = (WORD)( (DWORD)m_wState * PKTENC_LCG_A + PKTENC_LCG_B );
	return m_wState;
}

inline WORD CPacketEncrypt::CipherOpCode( WORD wOpCode )
{
	return wOpCode ^ ( Tick() & 0xFF );
}

inline void CPacketEncrypt::Cipher( BYTE* pBuf, int nLen )
{
	for( int i = 0; i < nLen; i++ )
		pBuf[i] ^= (BYTE)( Tick() & 0xFF );
}
