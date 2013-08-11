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
// server.h

//#include "../game/q_shared.h"
//#include "../qcommon/qcommon.h"
//#include "../game/g_public.h"
//#include "../game/bg_public.h"

//=============================================================================

#define	MAX_ENT_CLUSTERS	16

// MAX_CHALLENGES is made large to prevent a denial
// of service attack that could cycle all of them
// out before legitimate users connected
#define	MAX_CHALLENGES	2048
// Allow a certain amount of challenges to have the same IP address
// to make it a bit harder to DOS one single IP address from connecting
// while not allowing a single ip to grab all challenge resources
#define MAX_CHALLENGES_MULTI (MAX_CHALLENGES / 2)


#define	AUTHORIZE_TIMEOUT	10000
#define	AUTHORIZE_TIMEOUT2	5000
#define DEDICATED

#ifndef AUTHORIZE_SERVER_NAME
#define	AUTHORIZE_SERVER_NAME	"cod4master.activision.com"
#endif

#ifndef PORT_AUTHORIZE
#define	PORT_AUTHORIZE		20800
#endif

#define	PORT_SERVER		28960

#define CLIENT_BASE_ADDR 0x90b4f8C
#define cmd_functions_ADDR 0x887eb98

#define STRBUFFBASEPTR_ADDR 0x897d780

#define RCON_RETURN_ADDR 0x13e78438

#define	MAX_MSGLEN	0x20000		// max length of a message, which may
#define	MAC_STATIC			// be fragmented into multiple packets

#define TRUNCATE_LENGTH	64
#define MAXPRINTMSG 4096

#define SV_OUTPUTBUF_LENGTH (8192 - 16)


#define CL_DECODE_START 0
//#define SV_DECODE_START 12
//#define SV_ENCODE_START 4
#define CL_ENCODE_START 9


/*

Some Info:
svs.nextSnapshotEntities 0x13f18f94
svs.numSnapshotEntites 0x13f18f8c
svc_snapshot = 6;
svs.snapflagServerbit 0x13f18f88  //copied from real svs. to something else
*/


unsigned int sys_timeBase;



// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

//============================================================================


/*
==============================================================

NET

==============================================================
*/

#define NET_ENABLEV4            0x01
#define NET_ENABLEV6            0x02
// if this flag is set, always attempt ipv6 connections instead of ipv4 if a v6 address is found.
#define NET_PRIOV6              0x04
// disables ipv6 multicast support if set.
#define NET_DISABLEMCAST        0x08
#define	PORT_ANY		-1

typedef enum {
	NA_BAD = 0,					// an address lookup failed
	NA_BOT = 0,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IP6 = 5,
	NA_TCP = 6,
	NA_TCP6 = 7,
	NA_MULTICAST6 = 8,
	NA_UNSPEC = 9,
	NA_DOWN = 10,
} netadrtype_t;

typedef enum {
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

#define NET_ADDRSTRMAXLEN 48	// maximum length of an IPv6 address string including trailing '\0'

typedef struct {
	byte	type;
	byte	scope_id;
	unsigned short	port;
	short sock;		//Socket FD. 0 = any socket
        union{
	    byte	ip[4];
	    byte	ipx[10];
	    byte	ip6[16];
	};
}netadr_t;


void		NET_Init( void );
void		NET_Shutdown( void );
void		NET_Restart_f( void );
void		NET_Config( qboolean enableNetworking );
void		NET_FlushPacketQueue(void);
qboolean	NET_SendPacket (netsrc_t sock, int length, const void *data, netadr_t *to);
void		QDECL NET_OutOfBandPrint( netsrc_t net_socket, netadr_t *adr, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void		QDECL NET_OutOfBandData( netsrc_t sock, netadr_t *adr, byte *format, int len );
void		QDECL NET_PrintData( int *sock, const char *format, ... );
void		NET_SendData( int *sock, byte *data, int len );
void		NET_RegisterDefaultCommunicationSocket(netadr_t *adr);
netadr_t*	NET_GetDefaultCommunicationSocket();
qboolean	NET_CompareAdr (netadr_t *a, netadr_t *b);
qboolean	NET_CompareBaseAdrMask(netadr_t *a, netadr_t *b, int netmask);
qboolean	NET_CompareBaseAdr (netadr_t *a, netadr_t *b);
qboolean	NET_IsLocalAddress (netadr_t adr);
const char	*NET_AdrToString (netadr_t *a);
const char	*NET_AdrToStringShort (netadr_t *a);
const char	*NET_AdrToStringwPort (netadr_t *a);
netadr_t	*NET_SockToAdr(int socket);
const char	*NET_AdrToConnectionString(netadr_t *a);
const char	*NET_AdrToConnectionStringShort(netadr_t *a);
const char	*NET_AdrToConnectionStringMask(netadr_t *a);
int		NET_StringToAdr ( const char *s, netadr_t *a, netadrtype_t family);
qboolean	NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, msg_t *net_message);
void		NET_JoinMulticast6(void);
void		NET_LeaveMulticast6(void);
__optimize3 __regparm1 qboolean	NET_Sleep(unsigned int usec);
void NET_Clear(void);
const char*	NET_AdrMaskToString(netadr_t *adr);
void		NET_TcpPacketEventLoop();
qboolean	Sys_SendPacket( int length, const void *data, netadr_t *to );

qboolean	Sys_StringToAdr( const char *s, netadr_t *a, netadrtype_t family );

//Does NOT parse port numbers, only base addresses.
qboolean	Sys_IsLANAddress (netadr_t *adr);
void		Sys_ShowIP(void);




typedef enum {
	TCP_AUTHWAIT,
	TCP_AUTHNOTME,
	TCP_AUTHBAD,
	TCP_AUTHAGAIN,
	TCP_AUTHSUCCESSFULL
}tcpclientstate_t;


/*
==============================================================

MATHLIB

==============================================================
*/


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;


/*
==============================================================

COLLISION DETECTION

==============================================================
*/

// plane types are used to speed some tests
// 0-2 are axial planes
#define PLANE_X         0
#define PLANE_Y         1
#define PLANE_Z         2
#define PLANE_NON_AXIAL 3


/*
=================
PlaneTypeForNormal
=================
*/

#define PlaneTypeForNormal( x ) ( x[0] == 1.0 ? PLANE_X : ( x[1] == 1.0 ? PLANE_Y : ( x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL ) ) )

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {
	vec3_t normal;
	float dist;
	byte type;              // for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte signbits;          // signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte pad[2];
} cplane_t;


// a trace is returned when a box is swept through the world
typedef struct {
	float	fraction;       //0x00 time completed, 1.0 = didn't hit anything
	int	unknown[6];
/*	qboolean allsolid;      // if true, plane is not valid
	qboolean startsolid;    // if true, the initial point was in a solid area
	float fraction;         // time completed, 1.0 = didn't hit anything			//0x00
	vec3_t endpos;          // final position
	cplane_t plane;         // surface normal at impact, transformed to world space
	int surfaceFlags;       // surface hit
	int contents;           // contents on other side of surface hit*/
	int	var_02;		//0x1c
	short	entityNum;      //0x20 entity the contacted sirface is a part of
} trace_t;

// trace->entityNum can also be 0 to (MAX_GENTITIES-1)
// or ENTITYNUM_NONE, ENTITYNUM_WORLD

typedef int clipHandle_t;
typedef int fileHandle_t;

/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/
#define ANIM_BITS       10

#define ANGLE2SHORT( x )  ( (int)( ( x ) * 65536 / 360 ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0 / 65536 ) )

#define SNAPFLAG_RATE_DELAYED   1
#define SNAPFLAG_NOT_ACTIVE     2   // snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT    4   // toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define MAX_CLIENTS         64 // JPW NERVE back to q3ta default was 128		// absolute limit
#define MAX_LOCATIONS       64

/***************** Verified *******************************/

#define GENTITYNUM_BITS     10  // JPW NERVE put q3ta default back for testing	// don't need to send any more
//#define	GENTITYNUM_BITS		11		// don't need to send any more		(SA) upped 4/21/2001 adjusted: tr_local.h (802-822), tr_main.c (1501), sv_snapshot (206)
#define MAX_GENTITIES       ( 1 << GENTITYNUM_BITS )

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define ENTITYNUM_NONE      ( MAX_GENTITIES - 1 )

/**********************************************************/



#define ENTITYNUM_WORLD     ( MAX_GENTITIES - 2 )
#define ENTITYNUM_MAX_NORMAL    ( MAX_GENTITIES - 2 )


vec3_t vec3_origin = {0,0,0};


// markfragments are returned by CM_MarkFragments()
typedef struct {
	int firstPoint;
	int numPoints;
} markFragment_t;



typedef struct {
	vec3_t origin;
	vec3_t axis[3];
} orientation_t;


// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {//Not Known
	int			serverTime;
/*
	byte			u0;
	byte			u1;
	byte			u2;
	float			angle1;
	float			angle2;
	float			angle3;

	byte			u3;
*/
	int			unk4[4];

	byte			weapon;     // weapon
	byte			offHandIndex;
	signed char	forwardmove, rightmove, upmove;
	byte			unk[7];
} usercmd_t;

//	vec3_t			angles;


#define	MAX_RELIABLE_COMMANDS	128	// max string commands buffered for restransmit
#define MAX_DOWNLOAD_WINDOW	8	// max of eight download frames
#define MAX_DOWNLOAD_BLKSIZE	2048	// 2048 byte block chunks
#define MAX_PACKET_USERCMDS	32



#define	PROTOCOL_VERSION	6

#define	MAX_MAP_AREA_BYTES	32
#define	PACKET_BACKUP		32
#define PACKET_MASK ( PACKET_BACKUP - 1 )

#define NETCHAN_UNSENTBUFFER_SIZE 0x20000
#define NETCHAN_FRAGMENTBUFFER_SIZE 0x800

typedef void* xfunction_t;

struct entityState_s;
struct usercmd_s;


typedef struct {
	// sequencing variables
	int			outgoingSequence;
	netsrc_t		sock;
	int			dropped;			// between last packet and previous
	int			incomingSequence;

	//Remote address
	netadr_t		remoteAddress;			// (0x10)
	short			qport;				// qport value to write when transmitting (0x24)

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;	
	byte			*fragmentBuffer; // (0x30)
	int			fragmentBufferSize;

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	qboolean		unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte			*unsentBuffer; //(0x44)
	int			unsentBufferSize;
} netchan_t;


// bit field limits
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16

#define	MAX_PS_EVENTS			2

#define PS_PMOVEFRAMECOUNTBITS	6

#define MAX_MODELS 256
#define MAX_CONFIGSTRINGS 2442


typedef struct{
	char command[MAX_STRING_CHARS];
	int cmdTime;
	int cmdType;
}reliableCommands_t;

//static char globalbuffer[8192];


netadr_t	net_local_adr;

time_t realtime;


typedef enum {
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,    // evTime is still valid
	SE_CONSOLE, // evPtr is a char*
	SE_PACKET   // evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct {
	int evTime;
	sysEventType_t evType;
	int evValue, evValue2;
	int evPtrLength;                // bytes of data pointed to by evPtr, for journaling
	void            *evPtr;         // this must be manually freed if not NULL
} sysEvent_t;


typedef struct{
    const char*		fastfile;
    int			loadpriority;
    int			notknown;
}XZoneInfo;


// the svc_strings[] array in cl_parse.c should mirror this
//
// server to client
//
enum svc_ops_e {
	svc_nop,
	svc_gamestate,
	svc_configstring,           // [short] [string] only in gamestate messages
	svc_baseline,               // only in gamestate messages
	svc_serverCommand,          // [string] to be executed by client game module
	svc_download,               // [short] size [size bytes]
	svc_snapshot,
	svc_EOF,
};


//
// client to server
//
/*
enum clc_ops_e {
	clc_bad,
	clc_nop,
	clc_move,               // [[usercmd_t]
	clc_moveNoDelta,        // [[usercmd_t]
	clc_clientCommand,      // [string] message
	clc_EOF,
	clc_heartbeat
};

*/

enum clc_ops_e {
	clc_move,               // [[usercmd_t]
	clc_moveNoDelta,        // [[usercmd_t]
	clc_clientCommand,      // [string] message
	clc_EOF,
	clc_nop
};


typedef struct {
unsigned const char	*start;
unsigned const char	*end;
} patternseek_t;

#define	MAX_REDIRECT_SERVERS	4
#define	MAX_CONNECTWAITTIME	10


#define NYT HMAX                    /* NYT = Not Yet Transmitted */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype {
	struct  nodetype *left, *right, *parent; /* tree structure */
//	struct  nodetype *next, *prev; /* doubly-linked list */
//	struct  nodetype **head; /* highest ranked node in block */
	int weight;
	int symbol; //0x10
	struct  nodetype *next, *prev; /* doubly-linked list */
	struct  nodetype **head; /* highest ranked node in block */

} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct {
	int blocNode;
	int blocPtrs;

	node_t*     tree;
	node_t*     lhead;
	node_t*     ltail;
	node_t*     loc[HMAX + 1];
	node_t**    freelist;

	node_t nodeList[768];
	node_t*     nodePtrs[768];
} huff_t;

typedef struct {
	huff_t compressor;
	huff_t decompressor;
} huffman_t;

void    Huff_Compress( msg_t *buf, int offset );
void    Huff_Decompress( msg_t *buf, int offset );
void    Huff_Init( huffman_t *huff );
void    Huff_addRef( huff_t* huff, byte ch );
int     Huff_Receive( node_t *node, int *ch, byte *fin );
void    Huff_transmit( huff_t *huff, int ch, byte *fout );
//void    Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset );
void    Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset );
void    Huff_putBit( int bit, byte *fout, int *offset );
int     Huff_getBit( byte *fout, int *offset );

extern huffman_t clientHuffTables;

struct snapshotInfo_s;

void Field_Clear( field_t *edit );


typedef union{
    float f;
    char c;
    int i;
    qboolean b;
    byte by;
    void* p;
}universalArg_t;


//
// sv_client.c

qboolean Sys_RandomBytes( byte* string, int length );

char* Sys_ConsoleInput( void );

void QDECL Com_Printf( const char *fmt, ... );
void QDECL Com_PrintError( const char *fmt, ... );
void QDECL Com_PrintWarning( const char *fmt, ... );

void QDECL Sys_Error( const char *fmt, ... );

void QDECL Com_DPrintf( const char *fmt, ... );

void Com_RandomBytes(byte *buffer, int len );

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *, qboolean) );

void Com_EndRedirect (void);

void Com_PrintUDP(const char* msg, int len);

void HL2Rcon_SourceRconSendDataToEachClient(const byte* data, int len, int type);

void HL2Rcon_SourceRconSendConsole(const char* data, int len);
void HL2Rcon_SourceRconSendChat(const char* data, int clnum);

void Sys_Print( const char* msg );

void Cmd_SetCommandCompletionFunc( const char *command, completionFunc_t complete );

void Cvar_CompleteCvarName( char *args, int argNum );

void Netchan_Init( int qport );

int Sys_Milliseconds(void);
int Sys_Seconds(void);
unsigned long long Sys_Microseconds(void);


void QDECL G_LogPrintf( const char *fmt, ... );


qboolean Netchan_Transmit( netchan_t *chan, int length, const byte *data );
qboolean Netchan_TransmitNextFragment( netchan_t *chan );

qboolean Netchan_Process( netchan_t *chan, msg_t *msg );

//void Plugin_Init(void);

void	NV_LoadConfig(void);
void	NV_WriteConfig(void);

qboolean Cmd_SetPower(const char* , int);

#define NV_ConfigInit NV_LoadConfig
#define NV_ProcessBegin NV_LoadConfig
#define NV_ProcessEnd NV_WriteConfig

