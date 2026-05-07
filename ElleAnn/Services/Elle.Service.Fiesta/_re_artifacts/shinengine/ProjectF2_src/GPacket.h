// GPacket.h
// GPacket -- base game packet structure.
// All NC_ packets are prefixed with this header on the wire.
//
// _USE_GPACKET must be defined to enable GPacket wrapping.
// Defined in all server project configurations.

#pragma once

#include "typedef.h"

#ifdef _USE_GPACKET

#pragma pack( push, 1 )

//------------------------------------------------------------------
// PROTO_NC_HEADER
// 4-byte wire header prepended to every packet.
//   wLen    : byte count of everything AFTER this field
//             (i.e. sizeof opcode + sizeof payload)
//   wOpCode : NC_* value; cipher-transformed on WAN connections
//------------------------------------------------------------------
struct PROTO_NC_HEADER
{
	WORD	wLen;
	WORD	wOpCode;
};

//------------------------------------------------------------------
// GPACKET
// Convenience union over raw byte buffer + typed header access.
// nMaxDataLen is the maximum payload size (not including header).
//------------------------------------------------------------------
#define GPACKET_MAX_LEN		4096

struct GPACKET
{
	PROTO_NC_HEADER	header;
	BYTE			data[ GPACKET_MAX_LEN ];

	WORD	GetLen()    const	{ return header.wLen; }
	WORD	GetOpCode() const	{ return header.wOpCode; }
	int		GetDataLen()const	{ return (int)header.wLen - sizeof(WORD); }
	BYTE*	GetData()			{ return data; }

	void	SetLen( WORD wLen )		{ header.wLen    = wLen; }
	void	SetOpCode( WORD wOp )	{ header.wOpCode = wOp; }

	// Total wire size: header + payload
	int		WireSize() const
	{ return (int)sizeof(PROTO_NC_HEADER) + (int)header.wLen; }
};

#pragma pack( pop )

//------------------------------------------------------------------
// SendPacket helpers -- see sendpacket.h for full implementation.
//------------------------------------------------------------------
// Packet is always: [wLen(2)][wOpCode(2)][payload(wLen-2)]
// wLen field counts the opcode word + payload bytes.
// Example: no-payload packet has wLen=2 (opcode word only).
//------------------------------------------------------------------
inline void GPACKET_INIT( GPACKET* pPkt, WORD wOpCode, WORD wPayloadLen )
{
	pPkt->header.wLen    = sizeof(WORD) + wPayloadLen;	// opcode + payload
	pPkt->header.wOpCode = wOpCode;
}

#endif // _USE_GPACKET
