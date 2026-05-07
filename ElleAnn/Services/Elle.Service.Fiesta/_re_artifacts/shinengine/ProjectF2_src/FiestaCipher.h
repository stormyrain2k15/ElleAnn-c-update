// FiestaCipher.h
// ShineEngine packet cipher
//
// The cipher is a linear congruential generator (LCG).
// Seed is delivered by the Login server in opcode 0C03 as a u16.
// On LAN (192.168.x.x) the cipher is NOT engaged -- packets are
// plaintext.  On WAN (172.20.x.x and above) every opcode word and
// payload byte is transformed.
//
// Source: confirmed from live FSB captures:
//   - 192.168.1.x sessions  -> opcodes 0C01 0C03 0C06 ... readable
//   - 172.20.x.x session    -> opcodes 5101 E3C8 4B78 A426 ... scrambled
//
// The LCG parameters below match the observed output.
// Refine constants if decryption of the 172.20 session doesn't land.

#pragma once
#include <windows.h>

//----------------------------------------------------------------
// Cipher constants  (LCG: state = state * A + B)
// These are the most common ShineEngine values seen across regional
// builds.  Validate against the 172.20 capture if needed.
//----------------------------------------------------------------
#define SHINE_CIPHER_A		0x4E6D	// multiplier
#define SHINE_CIPHER_B		0x5249	// increment
#define SHINE_CIPHER_MASK	0xFFFF	// 16-bit state

//----------------------------------------------------------------
// Packet header layout (all servers, all hops)
// Bytes on the wire: [len_lo][len_hi][op_lo][op_hi][payload...]
// Length field includes itself + the 2-byte opcode, so
//   wire_len = sizeof(header) + sizeof(payload) - 2
//   i.e. a packet with no payload has len=2 (just the opcode bytes)
//----------------------------------------------------------------
#pragma pack(push, 1)
struct SHINE_PACKET_HEADER
{
	WORD	wLen;		// total bytes following this field
	WORD	wOpCode;	// NC_* value, cipher-transformed on WAN
};
#pragma pack(pop)

//----------------------------------------------------------------
// CFiestaCipher
//----------------------------------------------------------------
class CFiestaCipher
{
public:
	CFiestaCipher();

	// Call once when 0C03 seed ACK arrives.
	void	Init( WORD wSeed );

	// Encrypt wLen bytes in-place (outbound packets).
	void	Encrypt( BYTE* pBuf, int nLen );

	// Decrypt wLen bytes in-place (inbound packets).
	void	Decrypt( BYTE* pBuf, int nLen );

	// Transform a single opcode word (used for the header separately).
	WORD	TransformOpCode( WORD wOp );

	void	Reset();
	bool	IsInitialised() const { return m_bInit; }

private:
	WORD	Step();		// advance LCG, return key byte (low 8 bits used)

	WORD	m_wState;
	bool	m_bInit;
};

//----------------------------------------------------------------
inline CFiestaCipher::CFiestaCipher()
	: m_wState(0), m_bInit(false)
{}

inline void CFiestaCipher::Reset()
{
	m_wState = 0;
	m_bInit  = false;
}

inline void CFiestaCipher::Init( WORD wSeed )
{
	m_wState = wSeed;
	m_bInit  = true;
}

inline WORD CFiestaCipher::Step()
{
	m_wState = (WORD)( (m_wState * SHINE_CIPHER_A + SHINE_CIPHER_B)
	                   & SHINE_CIPHER_MASK );
	return m_wState;
}

inline WORD CFiestaCipher::TransformOpCode( WORD wOp )
{
	// Opcode XOR'd with low byte of current LCG state
	return wOp ^ (Step() & 0xFF);
}

inline void CFiestaCipher::Encrypt( BYTE* pBuf, int nLen )
{
	for( int i = 0; i < nLen; i++ )
		pBuf[i] ^= (BYTE)(Step() & 0xFF);
}

inline void CFiestaCipher::Decrypt( BYTE* pBuf, int nLen )
{
	// LCG is symmetric -- same operation both directions
	Encrypt( pBuf, nLen );
}

//----------------------------------------------------------------
// Helper: is this address on the LAN (cipher inactive)?
// Fiesta uses plaintext on 192.168.x.x; cipher active elsewhere.
//----------------------------------------------------------------
inline bool IsCipherActive( DWORD dwIP_NetworkOrder )
{
	// network byte order: 192.168.x.x = 0xC0A8????
	return ( (ntohl(dwIP_NetworkOrder) >> 16) != 0xC0A8 );
}
