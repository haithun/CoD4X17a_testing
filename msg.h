/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#ifndef __MSG_H__
#define __MSG_H__

#include "q_shared.h"
#include "entity.h"
#include "q_math.h"

typedef struct snapshotInfo_s{
	int clnum;
	struct client_t* cl;
	int var_01;
	qboolean var_02;
	byte var_03;
}snapshotInfo_t;

//
// msg.c
//
typedef struct {
	qboolean	overflowed;		//0x00
	qboolean	readonly;		//0x04
	byte		*data;			//0x08
	int		var_01;			//0x0c
	int		maxsize;		//0x10
	int		cursize;		//0x14
	int		var_02;			//0x18
	int		readcount;		//0x1c
	int		bit;			//0x20	// for bitwise reads and writes
	int		lastRefEntity;		//0x24
	int		var_04;			//0x28
} msg_t; //Size: 0x2c


struct clientState_s;
struct playerState_s;
struct usercmd_s;


void MSG_Init( msg_t *buf, byte *data, int length ) ;
void MSG_Clear( msg_t *buf ) ;
void MSG_BeginReading( msg_t *msg ) ;
void MSG_Copy(msg_t *buf, byte *data, int length, msg_t *src);
void MSG_WriteByte( msg_t *msg, int c ) ;
void MSG_WriteShort( msg_t *msg, int c ) ;
void MSG_WriteLong( msg_t *msg, int c ) ;
void MSG_WriteData( msg_t *buf, const void *data, int length ) ;
void MSG_WriteString( msg_t *sb, const char *s ) ;
void MSG_WriteBigString( msg_t *sb, const char *s ) ;
int MSG_ReadByte( msg_t *msg ) ;
int MSG_ReadShort( msg_t *msg ) ;
int MSG_ReadLong( msg_t *msg ) ;
char *MSG_ReadString( msg_t *msg ) ;
char *MSG_ReadStringLine( msg_t *msg ) ;
void MSG_ReadData( msg_t *msg, void *data, int len ) ;
void MSG_ClearLastReferencedEntity( msg_t *msg ) ;
void MSG_WriteDeltaEntity(struct snapshotInfo_s* snap, msg_t* msg, int time, entityState_t* from, entityState_t* to, int arg_6);
void MSG_WriteBit0( msg_t *msg ) ;
int MSG_WriteBitsNoCompress( int d, byte* src, byte* dst , int size);
void MSG_WriteVector( msg_t *msg, vec3_t c );

int __cdecl MSG_ReadBits( msg_t *msg, int numBits);
int __cdecl GetMinBitCount( unsigned int number );
void __cdecl MSG_WriteDeltaClient(struct snapshotInfo_s*, msg_t*, int, struct clientState_s*, struct clientState_s*, int);
void __regparm3 MSG_WriteDeltaField(struct snapshotInfo_s* , msg_t* , int, unsigned const char*, unsigned const char*, const void* netfield, int, unsigned char);
void __cdecl MSG_WriteDeltaPlayerstate(struct snapshotInfo_s* , msg_t* , int , struct playerState_s* , struct playerState_s*);
void __cdecl MSG_WriteEntityIndex(struct snapshotInfo_s*, msg_t*, int, int);
void __cdecl MSG_WriteBit1(msg_t*);
void __cdecl MSG_WriteBits(msg_t*, int bits, int bitcount);
void __cdecl MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, struct usercmd_s *from, struct usercmd_s o );
void __cdecl MSG_SetDefaultUserCmd( struct playerState_s *ps, struct usercmd_s *ucmd );
int __cdecl MSG_WriteBitsCompress( char dummy, const byte *datasrc, byte *buffdest, int bytecount);
void __cdecl MSG_WriteReliableCommandToBuffer( const char* source, char* destination,int length);

#endif

