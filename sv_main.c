/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/


#include "q_shared.h"
#include "qcommon_io.h"
#include "qcommon_mem.h"
#include "qcommon.h"
#include "cvar.h"
#include "cmd.h"
#include "msg.h"
#include "server.h"
#include "plugin_handler.h"
#include "net_game_conf.h"
#include "misc.h"
#include "g_sv_shared.h"
#include "g_shared.h"
#include "q_platform.h"
#include "punkbuster.h"
#include "sys_thread.h"
#include "sys_main.h"
#include "scr_vm.h"
#include "xassets.h"
#include "nvconfig.h"
#include "hl2rcon.h"

#include <string.h>
#include <stdarg.h>
#include <unistd.h>

cvar_t	*sv_protocol;
cvar_t	*sv_privateClients;		// number of clients reserved for password
cvar_t	*sv_hostname;
cvar_t	*sv_punkbuster;
cvar_t	*sv_minPing;
cvar_t	*sv_maxPing;
cvar_t	*sv_queryIgnoreMegs;
cvar_t	*sv_queryIgnoreTime;
cvar_t	*sv_privatePassword;		// password for the privateClient slots
cvar_t	*sv_allowDownload;
cvar_t	*sv_wwwDownload;
cvar_t	*sv_wwwBaseURL;
cvar_t	*sv_wwwDlDisconnected;
cvar_t	*sv_voice;
cvar_t	*sv_voiceQuality;
cvar_t	*sv_cheats;
cvar_t	*sv_rconPassword;		// password for remote server commands
cvar_t	*sv_reconnectlimit;		// minimum seconds between connect messages
cvar_t	*sv_padPackets;			// add nop bytes to messages
cvar_t	*sv_mapRotation;
cvar_t	*sv_mapRotationCurrent;
cvar_t	*sv_nextmap;
cvar_t	*sv_paused;
cvar_t	*sv_killserver;			// menu system can set to 1 to shut server down
cvar_t	*sv_timeout;			// seconds without any message while in game
cvar_t	*sv_connectTimeout;		// seconds without any message while connecting
cvar_t	*sv_zombieTime;			// seconds to sink messages after disconnect
cvar_t	*sv_consayname;
cvar_t	*sv_contellname;
cvar_t	*sv_password;
cvar_t	*g_motd;
cvar_t	*sv_modStats;
cvar_t	*sv_authorizemode;
cvar_t	*sv_showasranked;
cvar_t	*sv_statusfile;
cvar_t	*g_friendlyPlayerCanBlock;
cvar_t	*g_FFAPlayerCanBlock;
cvar_t	*sv_autodemorecord;
cvar_t	*sv_demoCompletedCmd;

cvar_t	*sv_master[MAX_MASTER_SERVERS];	// master server ip address
cvar_t	*g_mapstarttime;
cvar_t	*sv_uptime;


serverStaticExt_t	svse;	// persistant server info across maps
permServerStatic_t	psvs;	// persistant even if server does shutdown


#define SV_OUTPUTBUF_LENGTH 1024

/*
cvar_t	*sv_fps = NULL;			// time rate for running non-clients
cvar_t	*sv_timeout;			// seconds without any message
cvar_t	*sv_zombietime;			// seconds to sink messages after disconnect
cvar_t	*sv_maxclients;
cvar_t	*sv_showloss;			// report when usercmds are lost


cvar_t	*sv_mapname;
cvar_t	*sv_mapChecksum;
cvar_t	*sv_serverid;
cvar_t	*sv_minRate;
cvar_t	*sv_maxRate;

cvar_t	*sv_gametype;
cvar_t	*sv_pure;
cvar_t	*sv_floodProtect;
cvar_t	*sv_lanForceRate; // dedicated 1 (LAN) server forces local client rates to 99999 (bug #491)
#ifndef STANDALONE
cvar_t	*sv_strictAuth;
#endif

cvar_t	*sv_banFile;

serverBan_t serverBans[SERVER_MAXBANS];
int serverBansCount = 0;
*/


static netadr_t	master_adr[MAX_MASTER_SERVERS][2];


/*
=============================================================================

EVENT MESSAGES

=============================================================================
*/

/*
void SV_DumpReliableCommands( client_t *client, const char* cmd) {

	if(!com_developer || com_developer->integer < 2)
		return;


	char msg[1040];

	Com_sprintf(msg, sizeof(msg), "Cl: %i, Seq: %i, Time: %i, NotAck: %i, Len: %i, Msg: %s\n",
		client - svs.clients, client->reliableSequence, svs.time ,client->reliableSequence - client->reliableAcknowledge, strlen(cmd), cmd);

	Com_Printf("^5%s", msg);

	Sys_EnterCriticalSection(5);

	if ( com_logfile && com_logfile->integer ) {
    // TTimo: only open the qconsole.log if the filesystem is in an initialized state
    //   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
	    if ( !reliabledump && FS_Initialized()) {
			struct tm *newtime;
			time_t aclock;

			time( &aclock );
			newtime = localtime( &aclock );

			reliabledump = FS_FOpenFileWrite( "reliableCmds.log" );

			if ( com_logfile->integer > 1 && reliabledump) {
				// force it to not buffer so we get valid
				// data even if we are crashing
				FS_ForceFlush(reliabledump);
			}
			if ( reliabledump ) FS_Write(va("\nLogfile opened on %s\n", asctime( newtime )), strlen(va("\nLogfile opened on %s\n", asctime( newtime ))), reliabledump);
	    }
	    if ( reliabledump && FS_Initialized()) {
		FS_Write(msg, strlen(msg), reliabledump);
	    }
	}
	Sys_LeaveCriticalSection(5);
}
*/

/*
======================
SV_AddServerCommand

The given command will be transmitted to the client, and is guaranteed to
not have future snapshot_t executed before it is executed
======================
*/

/*

__cdecl void SV_AddServerCommand( client_t *client, int type, const char *cmd ) {

//	SV_DumpReliableCommands( client, cmd);
//	Com_Printf("C: %s\n", cmd);
	SV_AddServerCommand_old(client, type, cmd);
	return;


	int index, i;

	if(client->canNotReliable)
		return;

	if(client->state < CS_CONNECTED)
		return;

	if( ! *cmd )
		return;

//	extclient_t* extcl = &svse.extclients[ client - svs.clients ];
	client->reliableSequence++;

//	extcl->reliableSequence++;
//	Com_PrintNoRedirect("CMD: %i ^5%s\n", client->reliableSequence - client->reliableAcknowledge, cmd);
	// if we would be losing an old command that hasn't been acknowledged,
	// we must drop the connection
	// we check == instead of >= so a broadcast print added by SV_DropClient()
	// doesn't cause a recursive drop client
	if ( client->reliableSequence - client->reliableAcknowledge == MAX_RELIABLE_COMMANDS + 1 ) {

		Com_PrintNoRedirect("Client: %i Reliable commandbuffer overflow\n", client - svs.clients);
		Com_PrintNoRedirect( "Client lost reliable commands\n");
		Com_PrintNoRedirect( "===== pending server commands =====\n" );
		for ( i = client->reliableAcknowledge + 1 ; i <= client->reliableSequence ; i++ ) {
//			Com_DPrintNoRedirect( "cmd %5d: %s\n", i, extcl->reliableCommands[ i & (MAX_RELIABLE_COMMANDS-1) ].command );
			Com_PrintNoRedirect( "cmd %5d: %s\n", i, client->reliableCommands[ i & (MAX_RELIABLE_COMMANDS-1) ].command );
		}

		Com_PrintNoRedirect("cmd: %s\n", cmd);
		Com_PrintNoRedirect( "====================================\n" );
		SV_DropClient( client, "Server command overflow" );
		return;

	}
	index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );


//	MSG_WriteReliableCommandToBuffer(cmd, extcl->reliableCommands[ index ].command, sizeof( extcl->reliableCommands[ index ].command ));
//	extcl->reliableCommands[ index ].cmdTime = svs.time;
//	extcl->reliableCommands[ index ].cmdType = 1;

	MSG_WriteReliableCommandToBuffer(cmd, client->reliableCommands[ index ].command, sizeof( client->reliableCommands[ index ].command ));
	client->reliableCommands[ index ].cmdTime = svs.time;
	client->reliableCommands[ index ].cmdType = 1;

	SV_DumpReliableCommands( client, cmd);
}
*/

/*
=================
SV_SendServerCommand

Sends a reliable command string to be interpreted by
the client game module: "cp", "print", "chat", etc
A NULL client will broadcast to all clients
=================
*/
/*known stuff
"t \" ==  open callvote screen
"h \" ==  chat
"c \" ==  print bold to players screen
"e \" ==  print to players console
*/
void QDECL SV_SendServerCommand(client_t *cl, const char *fmt, ...) {
	va_list		argptr;
	byte		message[MAX_MSGLEN];
	client_t	*client;
	int		j;

	va_start (argptr,fmt);
	Q_vsnprintf ((char *)message, sizeof(message), fmt,argptr);
	va_end (argptr);

	if ( strlen ((char *)message) > 1022 ) {
		return;
	}

	if ( cl != NULL ){
		SV_AddServerCommand_old(cl, 0, (char *)message );
		return;
	}

	// hack to echo broadcast prints to console
	if ( !strncmp( (char *)message, "say", 3) ) {
		Com_Printf("broadcast: %s\n", SV_ExpandNewlines((char *)message) );
	}

	// send the data to all relevent clients
	for (j = 0, client = svs.clients; j < sv_maxclients->integer; j++, client++) {
		if ( client->state < CS_PRIMED ) {
			continue;
		}
		SV_AddServerCommand_old(client, 0, (char *)message );
	}
}


/*
==============================================================================

CONNECTIONLESS COMMANDS

==============================================================================
*/

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s {

	byte	type;

	union {
		byte	_4[4];
		byte	_6[16];
	} ipv;

	unsigned long long	lastTime;
	signed char	burst;

	long		hash;

	leakyBucket_t *prev, *next;
};


typedef struct{

    int max_buckets;
    int max_hashes;
    leakyBucket_t *buckets;
    leakyBucket_t **bucketHashes;
    int queryLimitsEnabled;
    leakyBucket_t infoBucket;
    leakyBucket_t statusBucket;
    leakyBucket_t rconBucket;
}queryLimit_t;


    static queryLimit_t querylimit;



// This is deliberately quite large to make it more of an effort to DoS

/*
================
SVC_RateLimitInit

Init the rate limit system
================
*/
static void SVC_RateLimitInit( ){

	int bytes;

	if(!sv_queryIgnoreMegs->integer)
	{
		Com_Printf("QUERY LIMIT: Querylimiting is disabled\n");
		querylimit.queryLimitsEnabled = 0;
		return;
	}

	bytes = sv_queryIgnoreMegs->integer * 1024*1024;

	querylimit.max_buckets = bytes / sizeof(leakyBucket_t);
	querylimit.max_hashes = 4096; //static

	int totalsize = querylimit.max_buckets * sizeof(leakyBucket_t) + querylimit.max_hashes * sizeof(leakyBucket_t*);

	querylimit.buckets = Z_Malloc(totalsize);

	if(!querylimit.buckets)
	{
		Com_PrintError("QUERY LIMIT: System is out of memory. All queries are disabled\n");
		querylimit.queryLimitsEnabled = -1;
	}

	querylimit.bucketHashes = (leakyBucket_t**)&querylimit.buckets[querylimit.max_buckets];
	Com_Printf("QUERY LIMIT: Querylimiting is enabled\n");
	querylimit.queryLimitsEnabled = 1;
}



/*
================
SVC_HashForAddress
================
*/
__optimize3 __regparm1 static long SVC_HashForAddress( netadr_t *address ) {
	byte 		*ip = NULL;
	size_t	size = 0;
	int			i;
	long		hash = 0;

	switch ( address->type ) {
		case NA_IP:  ip = address->ip;  size = 4; break;
		case NA_IP6: ip = address->ip6; size = 16; break;
		default: break;
	}

	for ( i = 0; i < size; i++ ) {
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) ^ psvs.randint);
	hash &= ( querylimit.max_hashes - 1 );

	return hash;
}

/*
================
SVC_BucketForAddress

Find or allocate a bucket for an address
================
*/
__optimize3 __regparm3 static leakyBucket_t *SVC_BucketForAddress( netadr_t *address, int burst, int period ) {
	leakyBucket_t		*bucket = NULL;
	int			i;
	long			hash = SVC_HashForAddress( address );
	unsigned long long	now = com_uFrameTime;

	for ( bucket = querylimit.bucketHashes[ hash ]; bucket; bucket = bucket->next ) {

		switch ( bucket->type ) {
			case NA_IP:
				if ( memcmp( bucket->ipv._4, address->ip, 4 ) == 0 ) {
					return bucket;
				}
				break;

			case NA_IP6:
				if ( memcmp( bucket->ipv._6, address->ip6, 16 ) == 0 ) {
					return bucket;
				}
				break;

			default:
				break;
		}

	}

	for ( i = 0; i < querylimit.max_buckets; i++ ) {
		int interval;

		bucket = &querylimit.buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) || interval < 0 ) ) 
		{
			if ( bucket->prev != NULL ) {
				bucket->prev->next = bucket->next;
			} else {
				querylimit.bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != NULL ) {
				bucket->next->prev = bucket->prev;
			}

			Com_Memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == NA_BAD ) {
			bucket->type = address->type;
			switch ( address->type ) {
				case NA_IP:	bucket->ipv._4[0] = address->ip[0]; 
						bucket->ipv._4[1] = address->ip[1];
						bucket->ipv._4[2] = address->ip[2];
						bucket->ipv._4[3] = address->ip[3];
						break;

				case NA_IP6: Com_Memcpy( bucket->ipv._6, address->ip6, 16 ); break;

				default: return NULL;
			}

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = querylimit.bucketHashes[ hash ];
			if ( querylimit.bucketHashes[ hash ] != NULL ) {
				querylimit.bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = NULL;
			querylimit.bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}


/*
================
SVC_RateLimit
================
*/
/*__optimize3*/ __attribute__((always_inline)) static qboolean SVC_RateLimit( leakyBucket_t *bucket, int burst, int period ) {
	if ( bucket != NULL ) {
		unsigned long long now = com_uFrameTime;
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst ) {
			bucket->burst = 0;
			bucket->lastTime = now;
		} else {
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst ) {
			bucket->burst++;

			return qfalse;
		}
	}

	return qtrue;
}

/*
================
SVC_RateLimitAddress

Rate limit for a particular address
================
*/
__optimize3 __regparm3 static qboolean SVC_RateLimitAddress( netadr_t *from, int burst, int period ) {

	if(Sys_IsLANAddress(from))
		return qfalse;


	if(querylimit.queryLimitsEnabled == 1)
	{
		leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );
		return SVC_RateLimit( bucket, burst, period );

	}else if(querylimit.queryLimitsEnabled == 0){
		return qfalse;

	}else{//Init error, to be save we deny everything
		return qtrue;
	}

}


/*
================
SVC_Status

Responds with all the info that qplug or qspy can see about the server
and all connected players.  Used for getting detailed information after
the simple info query.
================
*/

__optimize3 __regparm1 void SVC_Status( netadr_t *from ) {
	char player[1024];
	char status[MAX_MSGLEN];
	int i;
	client_t    *cl;
	gclient_t *gclient;
	int statusLength;
	int playerLength;
	char infostring[MAX_INFO_STRING];


	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &querylimit.statusBucket, 20, 20000 ) ) {
	//	Com_DPrintf( "SVC_Status: overall rate limit exceeded, dropping request\n" );
		return;
	}

	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress( from, 2, sv_queryIgnoreTime->integer*1000 ) ) {
	//	Com_DPrintf( "SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString( *from ) );
		return;
	}


	if(strlen(SV_Cmd_Argv(1)) > 128)
		return;

	strcpy( infostring, Cvar_InfoString( 0, (CVAR_SERVERINFO | CVAR_NORESTART)) );
	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", SV_Cmd_Argv( 1 ) );

	if(*sv_password->string)
	    Info_SetValueForKey( infostring, "pswrd", "1");

	if(sv_authorizemode->integer == 1)		//Backward compatibility
		Info_SetValueForKey( infostring, "type", "1");
	else
		Info_SetValueForKey( infostring, "type", va("%i", sv_authorizemode->integer));
	// add "demo" to the sv_keywords if restricted

	status[0] = 0;
	statusLength = 0;

	for ( i = 0, gclient = level.clients ; i < sv_maxclients->integer ; i++, gclient++ ) {
		cl = &svs.clients[i];
		if ( cl->state >= CS_CONNECTED ) {
			Com_sprintf( player, sizeof( player ), "%i %i \"%s\"\n",
						 gclient->pers.scoreboard.score, cl->ping, cl->name );
			playerLength = strlen( player );
			if ( statusLength + playerLength >= sizeof( status ) ) {
				break;      // can't hold any more
			}
			strcpy( status + statusLength, player );
			statusLength += playerLength;
		}
	}
	NET_OutOfBandPrint( NS_SERVER, from, "statusResponse\n%s\n%s", infostring, status );
}


/*
================
SVC_Info

Responds with a short info message that should be enough to determine
if a user is interested in a server to do a full status
================
*/
__optimize3 __regparm1 void SVC_Info( netadr_t *from ) {
	int		i, count, humans;
	qboolean	masterserver;
	char		infostring[MAX_INFO_STRING];
	char*		s;

	s = SV_Cmd_Argv(1);
	masterserver = qfalse;

	if(from->type == NA_IP)
	{
		for(i = 0; i < MAX_MASTER_SERVERS; i++)
		{
			if(NET_CompareAdr( from, &master_adr[i][0]))
			{
				masterserver = qtrue;
				break;
			}
		}


	}else{
		for(i = 0; i < MAX_MASTER_SERVERS; i++)
		{
			if(NET_CompareAdr( from, &master_adr[i][1]))
			{
				masterserver = qtrue;
				break;
			}
		}

	}

	if(!masterserver)
	{
		// Allow getstatus to be DoSed relatively easily, but prevent
		// excess outbound bandwidth usage when being flooded inbound
		if ( SVC_RateLimit( &querylimit.infoBucket, 100, 100000 ) ) {
		//	Com_DPrintf( "SVC_Info: overall rate limit exceeded, dropping request\n" );
			return;
		}


		// Prevent using getstatus as an amplifier
		if ( SVC_RateLimitAddress( from, 4, sv_queryIgnoreTime->integer*1000 )) {
		//	Com_DPrintf( "SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString( *from ) );
			return;
		}
		infostring[0] = 0;
	}else{
		infostring[0] = 0;
		Info_SetValueForKey( infostring, "server_id", va("%i", psvs.masterServer_id));
	}

	/*
	 * Check whether Cmd_Argv(1) has a sane length. This was not done in the original Quake3 version which led
	 * to the Infostring bug discovered by Luigi Auriemma. See http://aluigi.altervista.org/ for the advisory.
	 */

	// A maximum challenge length of 128 should be more than plenty.
	if(strlen(SV_Cmd_Argv(1)) > 128)
		return;

	// don't count privateclients
	count = humans = 0;
	for ( i = 0 ; i < sv_maxclients->integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED ) {
			count++;
			if (svs.clients[i].netchan.remoteAddress.type != NA_BOT) {
				humans++;
			}
		}
	}

	// echo back the parameter to status. so servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers

	Info_SetValueForKey( infostring, "challenge", s);


	//Info_SetValueForKey( infostring, "gamename", com_gamename->string );

	Info_SetValueForKey(infostring, "protocol", "6");

	Info_SetValueForKey( infostring, "hostname", sv_hostname->string );

	if(sv_authorizemode->integer == 1)		//Backward compatibility
		Info_SetValueForKey( infostring, "type", "1");
	else
		Info_SetValueForKey( infostring, "type", va("%i", sv_authorizemode->integer));

	Info_SetValueForKey( infostring, "mapname", sv_mapname->string );
	Info_SetValueForKey( infostring, "clients", va("%i", count) );
	Info_SetValueForKey( infostring, "g_humanplayers", va("%i", humans));
	Info_SetValueForKey( infostring, "sv_maxclients", va("%i", sv_maxclients->integer - sv_privateClients->integer ) );
	Info_SetValueForKey( infostring, "gametype", g_gametype->string );
	Info_SetValueForKey( infostring, "pure", va("%i", sv_pure->boolean ) );
	Info_SetValueForKey( infostring, "build", va("%i", BUILD_NUMBER));
	Info_SetValueForKey( infostring, "shortversion", Q3_VERSION );

        if(*sv_password->string)
	    Info_SetValueForKey( infostring, "pswrd", "1");
	else
	    Info_SetValueForKey( infostring, "pswrd", "0");

        if(g_cvar_valueforkey("scr_team_fftype")){
	    Info_SetValueForKey( infostring, "ff", va("%i", g_cvar_valueforkey("scr_team_fftype")));
	}

        if(g_cvar_valueforkey("scr_game_allowkillcam")){
	    Info_SetValueForKey( infostring, "ki", "1");
	}

        if(g_cvar_valueforkey("scr_hardcore")){
	    Info_SetValueForKey( infostring, "hc", "1");
	}

        if(g_cvar_valueforkey("scr_oldschool")){
	    Info_SetValueForKey( infostring, "od", "1");
	}
	Info_SetValueForKey( infostring, "hw", "1");

        if(fs_game->string[0] == '\0' || sv_showasranked->boolean){
	    Info_SetValueForKey( infostring, "mod", "0");
	}else{
	    Info_SetValueForKey( infostring, "mod", "1");
	}
	Info_SetValueForKey( infostring, "voice", va("%i", sv_voice->boolean ) );
	Info_SetValueForKey( infostring, "pb", va("%i", sv_punkbuster->boolean) );

	if( sv_maxPing->integer ) {
		Info_SetValueForKey( infostring, "sv_maxPing", va("%i", sv_maxPing->integer) );
	}

	if( fs_game->string[0] != '\0' ) {
		Info_SetValueForKey( infostring, "game", fs_game->string );
	}

	NET_OutOfBandPrint( NS_SERVER, from, "infoResponse\n%s", infostring );
}


/*
================
SVC_FlushRedirect

================
*/
static void SV_FlushRedirect( char *outputbuf, qboolean lastcommand ) {
	NET_OutOfBandPrint( NS_SERVER, &svse.redirectAddress, "print\n%s", outputbuf );
}

/*
===============
SVC_RemoteCommand

An rcon packet arrived from the network.
Shift down the remaining args
Redirect all printfs
===============
*/
__optimize3 __regparm2 static void SVC_RemoteCommand( netadr_t *from, msg_t *msg ) {
	// TTimo - scaled down to accumulate, but not overflow anything network wise, print wise etc.
	// (OOB messages are the bottleneck here)
	char		sv_outputbuf[SV_OUTPUTBUF_LENGTH];
	char *cmd_aux;

	svse.redirectAddress = *from;

	if ( strcmp (SV_Cmd_Argv(1), sv_rconPassword->string )) {
		//Send only one deny answer out in 100 ms
		if ( SVC_RateLimit( &querylimit.rconBucket, 1, 100 ) ) {
		//	Com_DPrintf( "SVC_RemoteCommand: rate limit exceeded for bad rcon\n" );
			return;
		}

		Com_Printf ("Bad rcon from %s\n", NET_AdrToString (from) );
		Com_BeginRedirect (sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);
		Com_Printf ("Bad rcon");
		Com_EndRedirect ();
		return;
	}

	if ( strlen( sv_rconPassword->string) < 8 ) {
		Com_BeginRedirect (sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);
		Com_Printf ("No rconpassword set on server or password is shorter than 8 characters.\n");
		Com_EndRedirect ();
		return;
	}

	//Everything fine, process the request

	MSG_BeginReading(msg);
	MSG_ReadLong(msg); //0xffffffff
	MSG_ReadLong(msg); //rcon

	cmd_aux = MSG_ReadStringLine(msg);

	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=543
	// get the command directly, "rcon <pass> <command>" to avoid quoting issues
	// extract the command by walking
	// since the cmd formatting can fuckup (amount of spaces), using a dumb step by step parsing

	while(cmd_aux[0]==' ')//Skipping space before the password
		cmd_aux++;

	if(cmd_aux[0]== '"')//Skipping the password
	{
		cmd_aux++;
		while(cmd_aux[0] != '"' && cmd_aux[0])
			cmd_aux++;

		cmd_aux++;
	}else{
		while(cmd_aux[0] != ' ' && cmd_aux[0])
			cmd_aux++;

	}

	while(cmd_aux[0] == ' ')//Skipping space after the password
		cmd_aux++;

	Com_Printf ("Rcon from %s: %s\n", NET_AdrToString (from), cmd_aux );

	Com_BeginRedirect (sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);

	if(!Q_stricmpn(cmd_aux, "pb_sv_", 6)){

		Q_strchrrepl(cmd_aux, '\"', ' ');
		Cmd_ExecuteSingleCommand(0,0, cmd_aux);
		PbServerForceProcess();
	}else{
		Cmd_ExecuteSingleCommand(0,0, cmd_aux);
	}
	Com_EndRedirect ();

}

/*
=================
SV_ConnectionlessPacket

A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
===========h======
*/
__optimize3 __regparm2 void SV_ConnectionlessPacket( netadr_t *from, msg_t *msg ) {
	char	*s;
	char	*c;
	int	clnum;
	int	i;
	client_t *cl;

	MSG_BeginReading( msg );
	MSG_ReadLong( msg );		// skip the -1 marker

	s = MSG_ReadStringLine( msg );
	SV_Cmd_TokenizeString( s );

	c = SV_Cmd_Argv(0);

	Com_DPrintf ("SV packet %s : %s\n", NET_AdrToString(from), c);
	//Most sensitive OOB commands first
        if (!Q_stricmp(c, "getstatus")) {
		SVC_Status( from );

        } else if (!Q_stricmp(c, "getinfo")) {
		SVC_Info( from );

        } else if (!Q_stricmp(c, "rcon")) {
		SVC_RemoteCommand( from, msg );

	} else if (!Q_strncmp("PB_", (char *) &msg->data[4], 3)) {
		//pb_sv_process here
		SV_Cmd_EndTokenizeString();

		if(msg->data[7] == 0x43 || msg->data[7] == 0x31 || msg->data[7] == 0x4a)
		    return;

		for ( i = 0, clnum = -1, cl = svs.clients ; i < sv_maxclients->integer ; i++, cl++ ){
			if ( !NET_CompareBaseAdr( from, &cl->netchan.remoteAddress ) )	continue;
			if(cl->netchan.remoteAddress.port != from->port) continue;
			clnum = i;
			break;
		}

		if(NET_GetDefaultCommunicationSocket() == NULL)
			NET_RegisterDefaultCommunicationSocket(from);

		PbSvAddEvent(13, clnum, msg->cursize-4, (char *)&msg->data[4]);
		return;

	} else if (!Q_stricmp(c, "connect")) {
		SV_DirectConnect( from );

	} else if (!Q_stricmp(c, "ipAuthorize")) {
		SV_AuthorizeIpPacket( from );

	} else if (!Q_stricmp(c, "stats")) {
		SV_ReceiveStats(from, msg);

        } else if (!Q_stricmp(c, "rcon")) {
		SVC_RemoteCommand( from, msg );

	} else if (!Q_stricmp(c, "getchallenge")) {
		SV_GetChallenge(from);

	} else {
		Com_DPrintf ("bad connectionless packet from %s\n", NET_AdrToString (from));
	}
	SV_Cmd_EndTokenizeString();
	return;
}




//============================================================================

/*
=================
SV_ReadPackets
=================
*/
__optimize3 __regparm2 void SV_PacketEvent( netadr_t *from, msg_t *msg ) {
	int i;
	client_t    *cl;
	short qport;

	if(!com_sv_running->boolean)
            return;

	// check for connectionless packet (0xffffffff) first
	if ( msg->cursize >= 4 && *(int *)msg->data == -1 ) {
		SV_ConnectionlessPacket( from, msg );
		return;
	}

	SV_ResetSekeletonCache();

	// read the qport out of the message so we can fix up
	// stupid address translating routers

	//InsertPluginEvent
/*
	seqclient_t* clq = CL_AddrToServer(from);
	if(clq)
	{
		CL_ReadPacket( clq ,msg );
		return;
	}
*/
	MSG_BeginReading( msg );
	MSG_ReadLong( msg );           // sequence number
	qport = MSG_ReadShort( msg );  // & 0xffff;

	// find which client the message is from
	for ( i = 0, cl = svs.clients ; i < sv_maxclients->integer ; i++,cl++ ) {
		if ( cl->state == CS_FREE ) {
			continue;
		}
		if ( !NET_CompareBaseAdr( from, &cl->netchan.remoteAddress ) ) {
			continue;
		}

		// it is possible to have multiple clients from a single IP
		// address, so they are differentiated by the qport variable
		if ( cl->netchan.qport != qport ) {
			continue;
		}

		// the IP port can't be used to differentiate them, because
		// some address translating routers periodically change UDP
		// port assignments
		if ( cl->netchan.remoteAddress.port != from->port ) {
			Com_Printf( "SV_PacketEvent: fixing up a translated port\n" );
			cl->netchan.remoteAddress.port = from->port;
		}
		// make sure it is a valid, in sequence packet
		if ( Netchan_Process( &cl->netchan, msg ) ) {
			// zombie clients still need to do the Netchan_Process
			// to make sure they don't need to retransmit the final
			// reliable message, but they don't do any other processing
			cl->serverId = MSG_ReadByte( msg );
			cl->messageAcknowledge = MSG_ReadLong( msg );

			if(cl->messageAcknowledge < 0){
				Com_Printf("Invalid reliableAcknowledge message from %s - reliableAcknowledge is %i\n", cl->name, cl->reliableAcknowledge);
				return;
			}
			cl->reliableAcknowledge = MSG_ReadLong( msg );

			if((cl->reliableSequence - cl->reliableAcknowledge) > (MAX_RELIABLE_COMMANDS - 1)){
				Com_Printf("Out of range reliableAcknowledge message from %s - reliableSequence is %i, reliableAcknowledge is %i\n",
				cl->name, cl->reliableSequence, cl->reliableAcknowledge);
				cl->reliableAcknowledge = cl->reliableSequence;
				return;
			}
			SV_Netchan_Decode(cl, &msg->data[msg->readcount], msg->cursize - msg->readcount);
			if ( cl->state != CS_ZOMBIE ) {
				cl->lastPacketTime = svs.time;  // don't timeout
				if(msg->cursize > 2000){
					//This will fix up a buffer overflow.
					//CoD4's message Decompress-function has no buffer overrun check
					//Because the compression algorithm is very poor this is already sufficent
					Com_Printf("Oversize message received from: %s\n", cl->name);
					SV_DropClient(cl, "Oversize client message");
				}else{

	//				SV_DumpCommands(cl, msg->data, msg->cursize, qtrue);
					SV_ExecuteClientMessage( cl, msg );
				}
			}
		}
		return;
	}
	// if we received a sequenced packet from an address we don't recognize,
	// send an out of band disconnect packet to it
	NET_OutOfBandPrint( NS_SERVER, from, "disconnect" );
}


/*
==============================================================================

MASTER SERVER FUNCTIONS

==============================================================================
*/

/*
================
SV_MasterHeartbeat

Send a message to the masters every few minutes to
let it know we are alive, and log information.
We will also have a heartbeat sent when a server
changes from empty to non-empty, and full to non-full,
but not on every player enter or exit.
================
*/
#define PORT_MASTER 20810
#define MASTER_SERVER_NAME "cod4master.activision.com"
#define MASTER_SERVER_NAME2 "cod4master.iceops.in"
#define HEARTBEAT_GAME "COD-4"
#define HEARTBEAT_DEAD "flatline"
#define	HEARTBEAT_USEC	180*1000*1000
void SV_MasterHeartbeat(const char *message)
{
	int			i;
	int			res;
	int			netenabled;

	netenabled = net_enabled->integer;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if (com_dedicated->integer != 2 || !(netenabled & (NET_ENABLEV4 | NET_ENABLEV6)))
		return;		// only dedicated servers send heartbeats

	// if not time yet, don't send anything
	if ( com_uFrameTime < svse.nextHeartbeatTime )
		return;

	svse.nextHeartbeatTime = com_uFrameTime + HEARTBEAT_USEC;

	// send to group masters
	for (i = 0; i < MAX_MASTER_SERVERS; i++)
	{
		if(!sv_master[i]->string[0])
			continue;

		// see if we haven't already resolved the name
		// resolving usually causes hitches on win95, so only
		// do it when needed
		if(sv_master[i]->modified || (master_adr[i][0].type == NA_BAD && master_adr[i][1].type == NA_BAD))
		{
			sv_master[i]->modified = qfalse;

			if(netenabled & NET_ENABLEV4)
			{
				Com_Printf("Resolving %s (IPv4)\n", sv_master[i]->string);
				//NA_IPANY For broadcasting to all interfaces
				res = NET_StringToAdr(sv_master[i]->string, &master_adr[i][0], NA_IP);

				if(res == 2)
				{
					// if no port was specified, use the default master port
					master_adr[i][0].port = BigShort(PORT_MASTER);
				}
				master_adr[i][0].sock = 0;

				if(res)
					Com_Printf( "%s resolved to %s\n", sv_master[i]->string, NET_AdrToString(&master_adr[i][0]));
				else
					Com_Printf( "%s has no IPv4 address.\n", sv_master[i]->string);
			}

			if(netenabled & NET_ENABLEV6)
			{
				Com_Printf("Resolving %s (IPv6)\n", sv_master[i]->string);
				res = NET_StringToAdr(sv_master[i]->string, &master_adr[i][1], NA_IP6);

				if(res == 2)
				{
					// if no port was specified, use the default master port
					master_adr[i][1].port = BigShort(PORT_MASTER);
				}

				master_adr[i][1].sock = 0;

				if(res)
					Com_Printf( "%s resolved to %s\n", sv_master[i]->string, NET_AdrToString(&master_adr[i][1]));
				else
					Com_Printf( "%s has no IPv6 address.\n", sv_master[i]->string);
			}

			if(master_adr[i][0].type == NA_BAD && master_adr[i][1].type == NA_BAD)
			{
				// if the address failed to resolve, clear it
				// so we don't take repeated dns hits
				Com_Printf("Couldn't resolve address: %s\n", sv_master[i]->string);
				Cvar_SetString(sv_master[i], "");
				sv_master[i]->modified = qfalse;
				continue;
			}
		}


		Com_Printf ("Sending heartbeat to %s\n", sv_master[i]->string );

		// this command should be changed if the server info / status format
		// ever incompatably changes
		if(master_adr[i][0].type != NA_BAD)
			NET_OutOfBandPrint( NS_SERVER, &master_adr[i][0], "heartbeat %s\n", message);
		if(master_adr[i][1].type != NA_BAD)
			NET_OutOfBandPrint( NS_SERVER, &master_adr[i][1], "heartbeat %s\n", message);
	}
}

/*
=================
SV_MasterShutdown

Informs all masters that this server is going down
=================
*/
void SV_MasterShutdown( void ) {
	// send a hearbeat right now
	svse.nextHeartbeatTime = 0;
	SV_MasterHeartbeat(HEARTBEAT_DEAD);

	// send it again to minimize chance of drops
	svse.nextHeartbeatTime = 0;
	SV_MasterHeartbeat(HEARTBEAT_DEAD);

	// when the master tries to poll the server, it won't respond, so
	// it will be removed from the list
}

/*
==================
SV_FinalMessage

Used by SV_Shutdown to send a final message to all
connected clients before the server goes down.  The messages are sent immediately,
not just stuck on the outgoing message list, because the server is going
to totally exit after returning from this function.
==================
*/
void SV_FinalMessage( const char *message, qboolean arg2 ) {
	int i, j;
	client_t    *cl;


	// send it twice, ignoring rate
	for ( j = 0 ; j < 2 ; j++ ) 
	{


		for ( i = 0, cl = svs.clients ; i < sv_maxclients->integer ; i++, cl++ ) 
		{
			if ( cl->state == CS_ACTIVE ) {
				// don't send a disconnect to a local client
				if ( cl->netchan.remoteAddress.type != NA_LOOPBACK ) {

					if(arg2)
						SV_SendServerCommand( cl, "%c \"%s\"", 0x77, message );
					else
						SV_SendServerCommand( cl, "%c \"%s^7 %s\" PB", 0x77, cl->name ,message);
				}
				// force a snapshot to be sent
				cl->nextSnapshotTime = -1;
				SV_SendClientSnapshot( cl );

			}else if( cl->state >= CS_CONNECTED ){

				NET_OutOfBandPrint(NS_SERVER, &cl->netchan.remoteAddress, "disconnect %s", message);

			}
		}

	}
}


void SV_DisconnectAllClients(){

	int i;
	client_t    *cl;

	for ( i = 0, cl = svs.clients ; i < sv_maxclients->integer ; i++, cl++ ) 
	{

		if( cl->state >= CS_CONNECTED ){

			SV_DropClient(cl, "EXE_DISCONNECTED");

		}
	}
}



/*
================
SV_ClearServer
================
*/
void SV_ClearServer( void ) {
	int i;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ ) {
		if ( sv.configstringIndex[i] ) {
			SL_RemoveRefToString( sv.configstringIndex[i] );
		}
	}

	if(sv.unkConfigIndex)
	{
		SL_RemoveRefToString(sv.unkConfigIndex);
	}

	Com_Memset( &sv, 0, sizeof( sv ) );
}




/*
================
SV_Shutdown

Called when each game quits,
before Sys_Quit or Sys_Error
================
*/

__cdecl void SV_Shutdown( const char *finalmsg ) {

	qboolean var_01;

	if ( !com_sv_running || !com_sv_running->integer ) {
		return;
	}

	Com_Printf( "----- Server Shutdown -----\n" );
	Com_Printf( "\nWith the reason: %s\n", finalmsg );
        if(SEH_StringEd_GetString(finalmsg)){
            var_01 = qtrue;
        }

	SV_FinalMessage( finalmsg, var_01 );

//	SV_RemoveOperatorCommands();
	SV_MasterShutdown();
	SV_ShutdownGameProgs();
	SV_DisconnectAllClients();
	SV_DemoSystemShutdown();
	SV_FreeClients();

	// free current level
	SV_ClearServer();

//	InsertPluginEvent
/*
	CL_ShutdownConnections( );


*/

	// free server static data
	Cvar_SetBool( com_sv_running, qfalse );

	memset( &svs, 0, sizeof( svs ) );
	memset( &svse, 0, sizeof( svse ) );

	Com_Printf( "---------------------------\n" );

	// disconnect any local clients
//	CL_Disconnect( qfalse );

}


/*
=============================================================================

Writing the serverstatus out to a XML-File.
This can be usefull to display serverinfo on a website

=============================================================================
*/

void	serverStatus_WriteCvars(cvar_t const* cvar, void *var ){
    xml_t *xmlbase = var;

    if(cvar->flags & (CVAR_SERVERINFO | CVAR_NORESTART)){
        XML_OpenTag(xmlbase,"Data",2, "Name",cvar->name, "Value",Cvar_DisplayableValue(cvar));
        XML_CloseTag(xmlbase);
    }
}

void	serverStatus_Write(){

    xml_t xmlbase;
    char outputbuffer[32768];
    int i, c;
    client_t    *cl;
    gclient_t	*gclient;
    char	score[16];
    char	team[4];
    char	kills[16];
    char	deaths[16];
    char	assists[16];
    char	teamname[32];
    char	cid[4];
    char	ping[4];

    time_t	realtime = Com_GetRealtime();
    char *timestr = ctime(&realtime);
    timestr[strlen(timestr)-1]= 0;

    if(!*sv_statusfile->string) return;

    XML_Init(&xmlbase,outputbuffer,sizeof(outputbuffer), "ISO-8859-1");
    XML_OpenTag(&xmlbase,"B3Status",1,"Time",timestr);

        XML_OpenTag(&xmlbase,"Game",9,"CapatureLimit","", "FragLimit","", "Map",sv_mapname->string, "MapTime","", "Name","cod4", "RoundTime","", "Rounds","", "TimeLimit","", "Type",g_gametype->string);
            Cvar_ForEach(serverStatus_WriteCvars, &xmlbase);
        XML_CloseTag(&xmlbase);

        for ( i = 0, c = 0, cl = svs.clients; i < sv_maxclients->integer ; cl++, i++ ) {
            if ( cl->state >= CS_CONNECTED ) c++;
        }
        XML_OpenTag(&xmlbase, "Clients", 1, "Total",va("%i",c));

            for ( i = 0, cl = svs.clients, gclient = level.clients; i < sv_maxclients->integer ; i++, cl++, gclient++ ) {
                if ( cl->state >= CS_CONNECTED ){

                        Com_sprintf(cid,sizeof(cid),"%i", i);

                        if(cl->state == CS_ACTIVE){


                            Com_sprintf(team,sizeof(team),"%i", gclient->sess.sessionTeam);
                            Com_sprintf(score,sizeof(score),"%i", gclient->pers.scoreboard.score);
                            Com_sprintf(kills,sizeof(kills),"%i", gclient->pers.scoreboard.kills);
                            Com_sprintf(deaths,sizeof(deaths),"%i", gclient->pers.scoreboard.deaths);
                            Com_sprintf(assists,sizeof(assists),"%i", gclient->pers.scoreboard.assists);
                            Com_sprintf(ping,sizeof(ping),"%i", cl->ping);
                            switch(gclient->sess.sessionTeam){

                                case TEAM_RED:
                                    if(!Q_strncmp(g_TeamName_Axis->string,"MPUI_SPETSNAZ", 13))
                                        Q_strncpyz(teamname, "Spetsnaz", 32);
                                    else if(!Q_strncmp(g_TeamName_Axis->string,"MPUI_OPFOR", 10))
                                        Q_strncpyz(teamname, "Opfor", 32);
                                    else
                                        Q_strncpyz(teamname, g_TeamName_Axis->string, 32);

                                    break;

                                case TEAM_BLUE:
                                    if(!Q_strncmp(g_TeamName_Allies->string,"MPUI_MARINES", 12))
                                        Q_strncpyz(teamname, "Marines", 32);
                                    else if(!Q_strncmp(g_TeamName_Allies->string,"MPUI_SAS", 8))
                                        Q_strncpyz(teamname, "S.A.S.", 32);
                                    else
                                        Q_strncpyz(teamname, g_TeamName_Allies->string,32);

                                    break;

                                case TEAM_FREE:
                                    Q_strncpyz(teamname, "Free",32);
                                    break;
                                case TEAM_SPECTATOR:
                                    Q_strncpyz(teamname, "Spectator", 32);
                                    break;
                                default:
                                    *teamname = 0;
                            }
                        }else{
                            *team = 0;
                            *score = 0;
                            *kills = 0;
                            *deaths = 0;
                            *assists = 0;
                            *ping = 0;
                            if(cl->state == CS_CONNECTED){
                                Q_strncpyz(teamname, "Connecting...", 32);
                            }else{
                                Q_strncpyz(teamname, "Loading...", 32);
                            }
                        }

                        XML_OpenTag(&xmlbase, "Client", 13, "CID",cid, "ColorName",cl->name, "DBID",va("%i", cl->uid), "IP",NET_AdrToStringShort(&cl->netchan.remoteAddress), "PBID",cl->pbguid, "Score",score, "Kills",kills, "Deaths",deaths, "Assists",assists, "Ping", ping, "Team",team, "TeamName", teamname, "Updated", timestr);
                        XML_CloseTag(&xmlbase);
                }
            }

        XML_CloseTag(&xmlbase);
    XML_CloseTag(&xmlbase);

    FS_SV_WriteFile(sv_statusfile->string, outputbuffer, strlen(outputbuffer));

}

/*
void SV_ValidateServerId()
{
    int res;

    Com_Printf("Resolving cod4master.iceops.in\n");

    res = NET_StringToAdr("cod4master.iceops.in", &psvs.masterServer_adr, NA_IP);
    if(res == 2)
    {
        psvs.masterServer_adr.port = BigShort(PORT_MASTER);
    }
    psvs.masterServer_adr.sock = 0;

    if(res)
    {
        Com_Printf("cod4master.iceops.in resolved to %s\n", NET_AdrToString(&psvs.masterServer_adr));
        NET_OutOfBandPrint(NS_SERVER, &psvs.masterServer_adr, "serverValidate CoD4 %d %s", psvs.masterServer_id, psvs.masterServer_challengepassword);
    }
    else
        Com_Printf("cod4master.iceops.in has no IPv4 address.\n");

}

void SV_ValidationResponse(netadr_t *from, msg_t* msg)
{
    int res;

    Com_Printf("Resolving cod4master.iceops.in\n");

    res = NET_StringToAdr("cod4master.iceops.in", &psvs.masterServer_adr, NA_IP);
    if(res == 2)
    {
        psvs.masterServer_adr.port = BigShort(PORT_MASTER);
    }
    psvs.masterServer_adr.sock = 0;

    if(res)
    {
        Com_Printf("cod4master.iceops.in resolved to %s\n", NET_AdrToString(&psvs.masterServer_adr));
        NET_OutOfBandPrint(NS_SERVER, &psvs.masterServer_adr, "serverValidate CoD4 %d %s", psvs.masterServer_id, psvs.masterServer_challengepassword);
    }
    else
        Com_Printf("cod4master.iceops.in has no IPv4 address.\n");

}
*/
void SV_InitServerId(){

/*    int read;
    char buf[256];
    *buf = 0;
    fileHandle_t file;

    FS_SV_FOpenFileRead("server_id.dat", &file);
    if(!file){
        Com_DPrintf("Couldn't open server_id.dat for reading\n");
        return;
    }
    Com_Printf( "Loading file server_id.dat\n");

    read = FS_ReadLine(buf,sizeof(buf),file);

    if(read == -1){
        Com_Printf("Can not read from server_id.dat\n");
        FS_FCloseFile(file);
        loadconfigfiles = qfalse;
        return;
    }


    psvs.masterServer_id = atoi(Info_ValueForKey(buf, "id"));
    Q_strncpyz(psvs.masterServer_challengepassword, Info_ValueForKey(buf, "challenge_password"), sizeof(psvs.masterServer_challengepassword));

    FS_FCloseFile(file);
*/

    Com_RandomBytes((byte*)&psvs.masterServer_id, sizeof(psvs.masterServer_id));
    psvs.masterServer_challengepassword[0] = '-';
    psvs.masterServer_challengepassword[1] = '1';
    psvs.masterServer_challengepassword[2] = '\0';
}




void SV_InitCvarsOnce(void){

	cvar_t** tmp;

	sv_paused = Cvar_RegisterBool("sv_paused", qfalse, CVAR_ROM, "True if the server is paused");
	sv_killserver = Cvar_RegisterBool("sv_killserver", qfalse, CVAR_ROM, "True if the server getting killed");
	sv_protocol = Cvar_RegisterInt("protocol", PROTOCOL_VERSION, PROTOCOL_VERSION, PROTOCOL_VERSION, 0x44, "Protocol version");
	sv_privateClients = Cvar_RegisterInt("sv_privateClients", 0, 0, 64, 4, "Maximum number of private clients allowed onto this server");
	sv_hostname = Cvar_RegisterString("sv_hostname", "^5CoD4Host", 5, "Host name of the server");
	sv_punkbuster = Cvar_RegisterBool("sv_punkbuster", qtrue, 0x15, "Enable PunkBuster on this server");
	sv_minPing = Cvar_RegisterInt("sv_minPing", 0, 0, 1000, 5, "Minimum allowed ping on the server");
	sv_maxPing = Cvar_RegisterInt("sv_maxPing", 0, 0, 1000, 5, "Maximum allowed ping on the server");
	sv_queryIgnoreMegs = Cvar_RegisterInt("sv_queryIgnoreMegs", 1, 0, 32, 0x11, "Number of megabytes of RAM to allocate for the querylimit IP-blacklist. 0 disables this feature");
	sv_queryIgnoreTime = Cvar_RegisterInt("sv_queryIgnoreTime", 2000, 100, 100000, 1, "How much milliseconds have to pass until another two queries are allowed");
	Cvar_RegisterBool("sv_disableClientConsole", qfalse, 4, "Disallow remote clients from accessing the client console");

	sv_privatePassword = Cvar_RegisterString("sv_privatePassword", "", 0, "Password for the private client slots");
	sv_rconPassword = Cvar_RegisterString("rcon_password", "", 0, "Password for the server remote control console");

	sv_allowDownload = Cvar_RegisterBool("sv_allowDownload", qtrue, 1, "Allow clients to download gamefiles from server");
	sv_wwwDownload = Cvar_RegisterBool("sv_wwwDownload", qfalse, 1, "Enable http download");
	sv_wwwBaseURL = Cvar_RegisterString("sv_wwwBaseURL", "", 1, "The base url to files for downloading from the HTTP-Server");
	sv_wwwDlDisconnected = Cvar_RegisterBool("sv_wwwDlDisconnected", qfalse, 1, "Should clients stay connected while downloading from a HTTP-Server?");

	sv_voice = Cvar_RegisterBool("sv_voice", qfalse, 0xd, "Allow serverside voice communication");
	sv_voiceQuality = Cvar_RegisterInt("sv_voiceQuality", 3, 0, 9, 8, "Voice quality");

	sv_cheats = Cvar_RegisterBool("sv_cheats", qfalse, 0x18, "Enable cheats on the server");
	sv_reconnectlimit = Cvar_RegisterInt("sv_reconnectlimit", 5, 0, 1800, 1, "Seconds to disallow a prior connected client to reconnect to the server");
	sv_padPackets = Cvar_RegisterInt("sv_padPackets", 0, 0, 0x7fffffff, 0, "How many nop-bytes to add to insert into each snapshot");

	sv_mapRotation = Cvar_RegisterString("sv_mapRotation", "", 0, "List of all maps server will play");
	sv_mapRotationCurrent = Cvar_RegisterString("sv_mapRotationCurrent", "", 0, "List of remaining maps server has to play");
	sv_nextmap = Cvar_RegisterString("nextmap", "", 0, "String to execute on map_rotate. This will override the default maprotation of CoD4 with the Quake3 style maprotation");

	sv_timeout = Cvar_RegisterInt("sv_timeout", 40, 0, 1800, 0, "Seconds to keep a client on server without a new clientmessage");
	sv_connectTimeout = Cvar_RegisterInt("sv_connectTimeout", 90, 0, 1800, 0, "Seconds to wait for a client which is loading a map without a new clientmessage");
	sv_zombieTime = Cvar_RegisterInt("sv_zombieTime", 2, 0, 1800, 0, "Seconds to keep a disconnected client on server to transmit the last message");

	Cvar_RegisterBool("clientSideEffects", qtrue, 0x80, "Enable loading _fx.gsc files on the client");

	sv_modStats = Cvar_RegisterBool("ModStats", qtrue, CVAR_ARCHIVE, "Flag whether to use stats of mod (when running a mod) or to use stats of the Cod4 coregame");
	sv_authorizemode = Cvar_RegisterInt("sv_authorizemode", 1, -1, 1, CVAR_ARCHIVE, "How to authorize clients, 0=acceptall(No GUIDs) 1=accept no one with invalid GUID");
	sv_showasranked = Cvar_RegisterBool("sv_showasranked", qfalse, CVAR_ARCHIVE, "List the server in serverlist of ranked servers even when it is modded");
	sv_statusfile = Cvar_RegisterString("sv_statusfilename", "serverstatus.xml", CVAR_ARCHIVE, "Filename to write serverstatus to disk");
	g_mapstarttime = Cvar_RegisterString("g_mapStartTime", "", CVAR_SERVERINFO | CVAR_ROM, "Time when current map has started");
	g_friendlyPlayerCanBlock = Cvar_RegisterBool("g_friendlyPlayerCanBlock", qfalse, CVAR_ARCHIVE, "Flag whether friendly players can block each other");
	g_FFAPlayerCanBlock = Cvar_RegisterBool("g_FFAPlayerCanBlock", qtrue, CVAR_ARCHIVE, "Flag whether players in non team based games can block each other");
	sv_password = Cvar_RegisterString("g_password", "", CVAR_ARCHIVE, "Password which is required to join this server");
	g_motd = Cvar_RegisterString("g_motd", "", CVAR_ARCHIVE, "Message of the day, which getting shown to every player on his 1st spawn");
	sv_uptime = Cvar_RegisterString("uptime", "", CVAR_SERVERINFO | CVAR_ROM, "Time the server is running since last restart");
	sv_autodemorecord = Cvar_RegisterBool("sv_autodemorecord", qfalse, CVAR_ARCHIVE, "Automatically start from each connected client a demo.");
	sv_demoCompletedCmd = Cvar_RegisterString("sv_demoCompletedCmd", "", CVAR_ARCHIVE, "This program will be executed when a demo has been completed. The demofilename will be passed as argument.");
	sv_consayname = Cvar_RegisterString("sv_consayname", "^2Server: ^7", CVAR_ARCHIVE, "If the server broadcast text-messages this name will be used");
	sv_contellname = Cvar_RegisterString("sv_contellname", "^5Server^7->^5PM: ^7", CVAR_ARCHIVE, "If the server broadcast text-messages this name will be used");

	sv_master[0] = Cvar_RegisterString("sv_master1", "", CVAR_ARCHIVE, "A masterserver name");
	sv_master[1] = Cvar_RegisterString("sv_master2", "", CVAR_ARCHIVE, "A masterserver name");
	sv_master[2] = Cvar_RegisterString("sv_master3", "", CVAR_ARCHIVE, "A masterserver name");
	sv_master[3] = Cvar_RegisterString("sv_master4", "", CVAR_ARCHIVE, "A masterserver name");
	sv_master[4] = Cvar_RegisterString("sv_master5", "", CVAR_ARCHIVE, "A masterserver name");

	if(sv_authorizemode->integer < 1)
		sv_master[5] = Cvar_RegisterString("sv_master6", "cod.iw4play.de", CVAR_ROM, "A masterserver name");
	else
		sv_master[5] = Cvar_RegisterString("sv_master6", "", CVAR_ARCHIVE, "A masterserver name");

	sv_master[6] = Cvar_RegisterString("sv_master7", MASTER_SERVER_NAME, CVAR_ROM, "Default masterserver name");
	sv_master[7] = Cvar_RegisterString("sv_master8", MASTER_SERVER_NAME2, CVAR_ROM, "Default masterserver name");

	tmp = (cvar_t**)(0x13ed89bc);
	*tmp = Cvar_RegisterString("g_gametype", "war", 0x24, "Current game type");
	tmp = (cvar_t**)(0x13ed8974);
	*tmp = Cvar_RegisterString("mapname", "", 0x44, "Current map name");
	tmp = (cvar_t**)(0x13ed8960);

	*tmp = Cvar_RegisterInt("sv_maxclients", 32, 1, 64, 0x25, "Maximum number of clients that can connect to a server");
	(*tmp)->max = (*tmp)->integer; //Don't allow to rise the slotcount above the initial value

	tmp = (cvar_t**)(0x13ed89c8);
	*tmp = Cvar_RegisterBool("sv_clientSideBullets", qtrue, 8, "If true, clients will synthesize tracers and bullet impacts");
	tmp = (cvar_t**)(0x13ed897c);
	*tmp = Cvar_RegisterInt("sv_maxRate", 100000, 0, 100000, 5, "Maximum allowed bitrate per client");
	tmp = (cvar_t**)(0x13ed89e4);
	*tmp = Cvar_RegisterInt("sv_floodprotect", 4, 0, 100000, 5, "Prevent malicious lagging by flooding the server with commands. Is the number of client commands allowed to process");
	tmp = (cvar_t**)(0x13ed89ec);
	*tmp = Cvar_RegisterBool("sv_showcommands", qfalse, 0, "Print all client commands");
	tmp = (cvar_t**)(0x13ed899c);
	*tmp = Cvar_RegisterString("sv_iwds", "", 0x48, "IWD server checksums");
	tmp = (cvar_t**)(0x13ed89a0);
	*tmp = Cvar_RegisterString("sv_iwdNames", "", 0x48, "Names of IWD files used by the server");
	tmp = (cvar_t**)(0x13ed89a4);
	*tmp = Cvar_RegisterString("sv_referencedIwds", "", 0x48, "Checksums of all referenced IWD files");
	tmp = (cvar_t**)(0x13ed89a8);
	*tmp = Cvar_RegisterString("sv_referencedIwdNames", "", 0x48, "Names of all referenced IWD files used by the server");
	tmp = (cvar_t**)(0x13ed89ac);
	*tmp = Cvar_RegisterString("sv_FFCheckSums", "", 0x48, "FastFile server checksums");
	tmp = (cvar_t**)(0x13ed89b0);
	*tmp = Cvar_RegisterString("sv_FFNames", "", 0x48, "Names of FastFiles used by the server");
	tmp = (cvar_t**)(0x13ed89b4);
	*tmp = Cvar_RegisterString("sv_referencedFFCheckSums", "", 0x48, "Checksums of all referenced FastFiles");
	tmp = (cvar_t**)(0x13ed89b8);
	*tmp = Cvar_RegisterString("sv_referencedFFNames", "", 0x48, "Names of all referenced FastFiles used by the server");
	tmp = (cvar_t**)(0x13ed8978);
	*tmp = Cvar_RegisterInt("sv_serverid", 0, 0x80000000, 0x7fffffff, 0x48, "Voice quality");
	tmp = (cvar_t**)(0x13ed89d0);
	*tmp = Cvar_RegisterBool("sv_pure", qtrue, 0xc, "Cannot use modified IWD files");
	tmp = (cvar_t**)(0x13ed8950);
	*tmp = Cvar_RegisterInt("sv_fps", 20, 1, 250, 0, "Server frames per second");
	tmp = (cvar_t**)(0x13ed89f4);
	*tmp = Cvar_RegisterBool("sv_showAverageBPS", qfalse, 0, "Show average bytes per second for net debugging");
	tmp = (cvar_t**)(0x13ed8a04);
	*tmp = Cvar_RegisterBool("sv_botsPressAttackBtn", qtrue, 0, "Allow testclients to press attack button");
	tmp = (cvar_t**)(0x13ed89c0);
	*tmp = Cvar_RegisterBool("sv_debugRate", qfalse, 0, "Enable snapshot rate debugging info");
	tmp = (cvar_t**)(0x13ed89c4);
	*tmp = Cvar_RegisterBool("sv_debugReliableCmds", qfalse, 0, "Enable debugging information for reliable commands");
	tmp = (cvar_t**)(0x13f19004);
	*tmp = Cvar_RegisterBool("sv_clientArchive", qtrue, 0, "Have the clients archive data to save bandwidth on the server");

}



void SV_Init(){

        SV_AddOperatorCommands();
        SV_InitCvarsOnce();
        SVC_RateLimitInit( );
        SV_InitBanlist();
        Init_CallVote();
        SV_RemoteCmdInit();
        SV_InitServerId();
        Com_RandomBytes((byte*)&psvs.randint, sizeof(psvs.randint));

}


/*
===================
SV_CalcPings

Updates the cl->ping variables
===================
*/
void SV_CalcPings( void ) {
	int i, j;
	client_t    *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->integer ; i++ ) {
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE ) {
			cl->ping = -1;
			continue;
		}
		if ( !cl->gentity ) {
			cl->ping = -1;
			continue;
		}
		if ( cl->netchan.remoteAddress.type == NA_BOT ) {
			cl->ping = 0;
			cl->lastPacketTime = svs.time;
			continue;
		}

		total = 0;
		count = 0;
		for ( j = 0 ; j < PACKET_BACKUP ; j++ ) {
			if ( cl->frames[j].messageAcked == 0xFFFFFFFF ) {
				continue;
			}
			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}
		if ( !count ) {
			cl->ping = 999;
		} else {
			cl->ping = total / count;
			if ( cl->ping > 999 ) {
				cl->ping = 999;
			}
		}
	}
}

/*
===============
SV_GetConfigstring

===============
*/
void SV_GetConfigstring( int index, char *buffer, int bufferSize ) {

	short strIndex;
	char* cs;

	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetConfigstring: bufferSize == %i", bufferSize );
	}
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		Com_Error( ERR_DROP, "SV_GetConfigstring: bad index %i\n", index );
	}
	strIndex = sv.configstringIndex[index];

	cs = SL_ConvertToString(strIndex);

	Q_strncpyz( buffer, cs, bufferSize );
}

typedef struct{
    int index;
    char* string;
    int unk1;
    int unk2;
}unkGameState_t;

#define unkGameStateStr (unkGameState_t*)UNKGAMESTATESTR_ADDR
#define UNKGAMESTATESTR_ADDR (0x826f260)
/*
===============

===============
*/

void SV_WriteGameState( msg_t* msg, client_t* cl ) {

	char* cs;
	int i, ebx, edi, esi, var_03, clnum;
	entityState_t nullstate, *base;
	snapshotInfo_t snapInfo;
	unkGameState_t *gsbase = unkGameStateStr;
	unkGameState_t *gsindex;
	unsigned short strindex;

	MSG_WriteByte( msg, svc_gamestate );
	MSG_WriteLong( msg, cl->reliableSequence );
	MSG_WriteByte( msg, svc_configstring );

	for ( esi = 0, edi = 0, var_03 = 0 ; esi < MAX_CONFIGSTRINGS ; esi++) {

		strindex = sv.configstringIndex[esi];
		gsindex = &gsbase[var_03];

		if(gsindex->index != esi){

			if(strindex != sv.unkConfigIndex)
				edi++;

			continue;
		}

		cs = SL_ConvertToString(strindex);

		if(gsindex->index > 820){
			if(Q_stricmp(gsindex->string, cs))
			{
				edi++;
			}
		}else{
			if(strcmp(gsindex->string, cs))
			{
				edi++;
			}
		}
		var_03++;
	}
	MSG_WriteShort(msg, edi);

	for ( ebx = 0, edi = -1, var_03 = 0 ; ebx < MAX_CONFIGSTRINGS ; ebx++) {

		gsindex = &gsbase[var_03];
		strindex = sv.configstringIndex[ebx];

		if(gsindex->index == ebx){
					//ebx and gsindex->index are equal
			var_03++;

			cs = SL_ConvertToString(strindex);

			if(ebx > 820){
				if(!Q_stricmp(gsindex->string, cs))
				{
					continue;
				}
			}else{
				if(!strcmp(gsindex->string, cs))
				{
					continue;
				}
			}

			if(sv.unkConfigIndex == strindex)
			{
				MSG_WriteBit0(msg);
				MSG_WriteBits(msg, gsindex->index, 12);
				MSG_WriteBigString(msg, "");
				edi = ebx;
				continue;
			}
		}

		strindex = sv.configstringIndex[ebx];
		if(sv.unkConfigIndex != strindex)
		{
			if(edi+1 == ebx){

				MSG_WriteBit1(msg);

			}else{

				MSG_WriteBit0(msg);
				MSG_WriteBits(msg, ebx, 12);
			}
			MSG_WriteBigString(msg, SL_ConvertToString(strindex));
		}
	}
	Com_Memset( &nullstate, 0, sizeof( nullstate ) );
	clnum = cl - svs.clients;

	// baselines
	for ( i = 0; i < MAX_GENTITIES ; i++ ) {
		base = &sv.svEntities[i].baseline;
		if ( !base->number ) {
			continue;
		}
		MSG_WriteByte( msg, svc_baseline );

		snapInfo.clnum = clnum;
		snapInfo.cl = NULL;
		snapInfo.var_01 = 0xFFFFFFFF;
		snapInfo.var_02 = qtrue;

		MSG_WriteDeltaEntity( &snapInfo, msg, 0, &nullstate, base, qtrue );
	}

	MSG_WriteByte( msg, svc_EOF );
}

/*
================
SV_RconStatusWrite

Used by rcon to retrive all serverdata as detailed as possible
================
*/
void SV_WriteRconStatus( msg_t* msg ) {

	//Reserve 19000 free bytes for msg_t struct

	int i;
	client_t    *cl;
	gclient_t *gclient;
	char infostring[MAX_INFO_STRING];

	infostring[0] = 0;

	if(!com_sv_running->boolean)
            return;

	strcpy( infostring, Cvar_InfoString( 0, (CVAR_SERVERINFO | CVAR_NORESTART)));
	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	if(*sv_password->string)
	    Info_SetValueForKey( infostring, "pswrd", "1");

	//Write teamnames
        if(!Q_strncmp(g_TeamName_Axis->string,"MPUI_SPETSNAZ", 13))
            Info_SetValueForKey( infostring, "axis", "Spetsnaz");
        else if(!Q_strncmp(g_TeamName_Axis->string,"MPUI_OPFOR", 10))
            Info_SetValueForKey( infostring, "axis", "Opfor");
        else
            Info_SetValueForKey( infostring, "axis", g_TeamName_Axis->string);

        if(!Q_strncmp(g_TeamName_Allies->string,"MPUI_MARINES", 12))
            Info_SetValueForKey( infostring, "allies", "Marines");
        else if(!Q_strncmp(g_TeamName_Allies->string,"MPUI_SAS", 8))
            Info_SetValueForKey( infostring, "allies", "S.A.S.");
        else
            Info_SetValueForKey( infostring, "allies", g_TeamName_Allies->string);

        Info_SetValueForKey( infostring, "svtime", va("%i", svs.time));

	//Writing general server info to msg (Reserve 1024 bytes)
	MSG_WriteString(msg, infostring);

	//Reserve 64 * 280 bytes = 18000
	//Writing clientinfo to msg
	for ( i = 0, gclient = level.clients ; i < sv_maxclients->integer ; i++, gclient++ ) {

		cl = &svs.clients[i];

		infostring[0] = 0;

		if ( cl->state >= CS_CONNECTED ) {
			MSG_WriteByte( msg, i );	//ClientIndex
			Info_SetValueForKey( infostring, "name", cl->name);
			Info_SetValueForKey( infostring, "uid", va("%i", cl->uid));
			Info_SetValueForKey( infostring, "pbguid", cl->pbguid);
			Info_SetValueForKey( infostring, "team", va("%i", gclient->sess.sessionTeam));
			Info_SetValueForKey( infostring, "score", va("%i", gclient->pers.scoreboard.score));
			Info_SetValueForKey( infostring, "kills", va("%i", gclient->pers.scoreboard.kills));
			Info_SetValueForKey( infostring, "deaths", va("%i", gclient->pers.scoreboard.deaths));
			Info_SetValueForKey( infostring, "assists", va("%i", gclient->pers.scoreboard.assists));
			Info_SetValueForKey( infostring, "ping", va("%i", cl->ping));

			if(cl->netchan.remoteAddress.type == NA_BOT)
				Info_SetValueForKey( infostring, "ipconn", "BOT");
			else
				Info_SetValueForKey( infostring, "ipconn", NET_AdrToConnectionString(&cl->netchan.remoteAddress));

			Info_SetValueForKey( infostring, "state", va("%i", cl->state));
			Info_SetValueForKey( infostring, "os", va("%c", cl->OS));
			Info_SetValueForKey( infostring, "power", va("%i", cl->power));
			Info_SetValueForKey( infostring, "rate", va("%i", cl->rate));
			MSG_WriteString(msg, infostring);
		}
	}
	MSG_WriteByte( msg, -1 );	//Terminating ClientIndex
}

void SV_GetServerStaticHeader(){
	svs.nextCachedSnapshotFrames = svsHeader.var_01;
	svs.nextCachedSnapshotEntities = svsHeader.var_03;
	svs.nextCachedSnapshotClients = svsHeader.var_04;
}



void SV_InitArchivedSnapshot(){

	svs.nextArchivedSnapshotFrames = 0;
	svs.nextArchivedSnapshotBuffer = 0;
	svs.nextCachedSnapshotEntities = 0;
	svs.nextCachedSnapshotEntities = 0;
	svs.nextCachedSnapshotFrames = 0;
}


void SV_RunFrame(){
	SV_ResetSekeletonCache();
	G_RunFrame(svs.time);
}


int SV_ClientAuthMode(){

	return sv_authorizemode->integer;

}

const char* SV_GetMessageOfTheDay(){

	return g_motd->string;

}


qboolean SV_FriendlyPlayerCanBlock(){

	return g_friendlyPlayerCanBlock->boolean;

}

qboolean SV_FFAPlayerCanBlock(){

	return g_FFAPlayerCanBlock->boolean;

}

//Few custom added things which should happen if we load a new level or restart a level
void SV_PreLevelLoad(){

	client_t* client;
	int i;

	Com_UpdateRealtime();
	time_t realtime = Com_GetRealtime();
	char *timestr = ctime(&realtime);
	timestr[strlen(timestr)-1]= 0;

	Sys_TermProcess(); //term childs

	Cvar_SetString(g_mapstarttime, timestr);

	PHandler_Event(PLUGINS_ONEXITLEVEL, NULL);
	SV_RemoveAllBots();
	SV_ReloadBanlist();

	NV_LoadConfig();

	G_InitMotd();

	if(sv_authorizemode->integer < 1){
		Cvar_SetString(sv_master[5], "cod.iw4play.de");
		sv_master[5]->flags = CVAR_ROM;
	}

	for ( client = svs.clients, i = 0 ; i < sv_maxclients->integer ; i++, client++ ) {

		G_DestroyAdsForPlayer(client); //Remove hud message ads

		// send the new gamestate to all connected clients
		if ( client->state < CS_ACTIVE ) {
			continue;
		}

		if ( client->netchan.remoteAddress.type == NA_BOT ) {
			continue;
		}

		if(SV_PlayerIsBanned(client->uid, client->pbguid, &client->netchan.remoteAddress)){
			SV_DropClient(client, "Prior kick/ban");
			continue;
		}
	}
	Pmove_ExtendedResetState();

	HL2Rcon_EventLevelStart();

}

void SV_PostLevelLoad(){
	PHandler_Event(PLUGINS_ONSPAWNSERVER, NULL);
	sv.frameusec = 1000000 / sv_fps->integer;
	sv.serverId = com_frameTime;
}


/*
==================
SV_Map

Restart the server on a different map
==================
*/
qboolean SV_Map( const char *levelname ) {
	char        *map;
	char mapname[MAX_QPATH];
	char expanded[MAX_QPATH];

	map = FS_GetMapBaseName(levelname);
	Q_strncpyz(mapname, map, sizeof(mapname));
	Q_strlwr(mapname);

	if(!useFastfiles->integer)
	{
		Com_sprintf(expanded, sizeof(expanded), "maps/mp/%s.d3dbsp", mapname);
		if ( FS_ReadFile( expanded, NULL ) == -1 ) {
			Com_PrintError( "Can't find map %s\n", expanded );
			return qfalse;
		}
	}

	if(!DB_FileExists(mapname, 0) && (!fs_game->string[0] || !DB_FileExists(mapname, 2))){
		Com_PrintError("Can't find map %s\n", mapname);
		if(!fs_game->string[0])
			Com_PrintError("A mod is required to run custom maps\n");
		return qfalse;
	}

//	Cbuf_ExecuteBuffer(0, 0, "selectStringTableEntryInDvar mp/didyouknow.csv 0 didyouknow");

	FS_ConvertPath(mapname);
	SV_PreLevelLoad();

	SV_SpawnServer(mapname);

	SV_PostLevelLoad();
	return qtrue;
}


void SV_PreFastRestart(){
	PHandler_Event(PLUGINS_ONPREFASTRESTART, NULL);
}
void SV_PostFastRestart(){
	PHandler_Event(PLUGINS_ONPOSTFASTRESTART, NULL);
}

/*
================
SV_MapRestart

Completely restarts a level, but doesn't send a new gamestate to the clients.
This allows fair starts with variable load times.
================
*/
void SV_MapRestart( qboolean fastRestart ){

	char mapname[MAX_QPATH];
	int i, j;
	client_t    *client;
	const char  *denied;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		Com_Printf( "Server is not running.\n" );
		return;
	}

	// DHM - Nerve :: Check for invalid gametype
	SV_SetGametype();
	if(Q_stricmp(sv.gametype, g_gametype->string)){
		fastRestart = qfalse; //No fastrestart if we have changed gametypes
	}
	Q_strncpyz(sv.gametype, g_gametype->string, sizeof(sv.gametype));
	int pers = G_GetSavePersist();


	if(!fastRestart)
	{
		G_SetSavePersist(0);
		Q_strncpyz(mapname, sv_mapname->string, sizeof(mapname));
		FS_ConvertPath(mapname);
		SV_PreLevelLoad();
		SV_SpawnServer(mapname);
		SV_PostLevelLoad();
		return;
	}

	SV_PreFastRestart();

	if(com_frameTime == sv.serverId)
		return;

	// connect and begin all the clients
	for ( client = svs.clients, i = 0 ; i < sv_maxclients->integer ; i++, client++ ) {

		G_DestroyAdsForPlayer(client); //Remove hud message ads

		// send the new gamestate to all connected clients
		if ( client->state < CS_CONNECTED ) {
			continue;
		}

		if ( client->netchan.remoteAddress.type == NA_BOT ) {
			continue;
		}

		// add the map_restart command
		NET_OutOfBandPrint( NS_SERVER, &client->netchan.remoteAddress, "fast_restart" );
	}

	SV_InitCvars();
	SV_InitArchivedSnapshot();

	svs.snapFlagServerBit ^= 4;

	sv_serverId = (0xf0 & sv_serverId) + ((sv_serverId + 1) & 0xf);

	Cvar_SetInt(sv_serverid, sv_serverId);

	sv.serverId = com_frameTime;

	sv.state = SS_LOADING;
	sv.restarting = qtrue;

	SV_RestartGameProgs(pers);

	// run a few frames to allow everything to settle
	for ( i = 0 ; i < 3 ; i++ ) {
		svs.time += 100;
		SV_RunFrame();
	}

	// connect and begin all the clients
	for ( i = 0, client = svs.clients; i < sv_maxclients->integer ; i++, client++ ) {

		// send the new gamestate to all connected clients
		if ( client->state < CS_CONNECTED ) {
			continue;
		}

		if ( client->netchan.remoteAddress.type == NA_BOT ) {
			continue;
		}

		if(!pers)
			j = -1;
		else
			j = 0;

		SV_AddServerCommand_old(client, 1, va("%c",((-44 & j) + 110) ) );

		// connect the client again, without the firstTime flag
		denied = ClientConnect(i, client->clscriptid);

		if(denied){
			SV_DropClient(client, denied);
			Com_Printf("SV_MapRestart: dropped client %i - denied!\n", i);
			continue;
		}

		if(client->state == CS_ACTIVE){
			SV_ClientEnterWorld( client, &client->lastUsercmd );
		}
	}

	// reset all the vm data in place without changing memory allocation
	// note that we do NOT set sv.state = SS_LOADING, so configstrings that
	// had been changed from their default values will generate broadcast updates
	sv.state = SS_GAME;
	sv.restarting = qfalse;
	SV_PostFastRestart();
}



/*
==================
SV_CheckTimeouts

If a packet has not been received from a client for timeout->integer
seconds, drop the conneciton.  Server time is used instead of
realtime to avoid dropping the local client while debugging.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
#define SV_MAXCS_CONNECTEDTIME 6

void SV_CheckTimeouts( void ) {
	int i;
	client_t    *cl;
	int primeddroppoint;
	int connectdroppoint;
	int activedroppoint;
	int zombiepoint;

	activedroppoint = svs.time - 1000 * sv_timeout->integer;
	primeddroppoint = svs.time - 1000 * sv_connectTimeout->integer;
	connectdroppoint = svs.time - 1000 * SV_MAXCS_CONNECTEDTIME;
	zombiepoint = svs.time - 1000 * sv_zombieTime->integer;

	for ( i = 0,cl = svs.clients ; i < sv_maxclients->integer ; i++,cl++ ) {
		// message times may be wrong across a changelevel
		if ( cl->lastPacketTime > svs.time ) {
			cl->lastPacketTime = svs.time;
		}

		if ( cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint ) {
			// using the client id cause the cl->name is empty at this point
			Com_DPrintf( "Going from CS_ZOMBIE to CS_FREE for client %d\n", i );
			cl->state = CS_FREE;    // can now be reused
			continue;
		}

		if(cl->state == CS_ACTIVE && cl->lastPacketTime < activedroppoint){
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 ) {
				SV_DropClient( cl, "EXE_TIMEDOUT" );
				cl->state = CS_FREE;    // don't bother with zombie state
			}
		} else if ( cl->state == CS_CONNECTED && cl->lastPacketTime < connectdroppoint ) {
			if ( ++cl->timeoutCount > 5 ) {
				SV_DropClient( cl, "EXE_TIMEDOUT" );
				cl->state = CS_FREE;    // don't bother with zombie state
			}
		} else if ( cl->state == CS_PRIMED && cl->lastPacketTime < primeddroppoint ) {
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 ) {
				SV_DropClient( cl, "EXE_TIMEDOUT" );
				cl->state = CS_FREE;    // don't bother with zombie state
			}
		} else {
			cl->timeoutCount = 0;
		}
	}
}

/*
==================
SV_CheckPaused
==================
*/
qboolean SV_CheckPaused( void ) {
	client_t    *cl;
	int i;

/*
	if ( !cl_paused->boolean ) {
		return qfalse;
	}
*/
	// only pause if there is not a single client connected
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->integer ; i++,cl++ ) {
		if ( cl->state >= CS_ZOMBIE && cl->netchan.remoteAddress.type != NA_BOT ) {
			if ( sv_paused->boolean ) {
				Cvar_SetBool( sv_paused, qfalse );
			}
			return qfalse;
		}
	}
	if ( !sv_paused->boolean ) {
		Cvar_SetBool( sv_paused, qtrue );
	}
	return qtrue;
}


/*
==================
SV_FrameMsec
Return time in millseconds until processing of the next server frame.
==================
*/
unsigned int SV_FrameUsec()
{
	if(sv_fps)
	{
		unsigned int frameUsec;
		
		frameUsec = 1000000 / sv_fps->integer;
		
		if(frameUsec < sv.timeResidual)
			return 0;
		else
			return frameUsec - sv.timeResidual;
	}
	else
		return 1;
}

/*
==================
SV_Frame

Player movement occurs as a result of packet events, which
happen before SV_Frame is called
==================
*/
__optimize3 __regparm1 qboolean SV_Frame( unsigned int usec ) {
	unsigned int frameUsec;
	char mapname[MAX_QPATH];
        client_t* client;
        int i;
        static qboolean underattack = qfalse;


	if ( !com_sv_running->boolean ) {
		usleep(100000);
		return qtrue;
	}


	// allow pause if only the local client is connected
/*	if ( SV_CheckPaused() ) {
		SV_MasterHeartbeat( HEARTBEAT_GAME );//Still send heartbeats
		CL_WritePacket( &svse.authserver );
		CL_WritePacket( &svse.scrMaster );
		return;
	}
*/
	// if it isn't time for the next frame, do nothing
	frameUsec = sv.frameusec * com_timescale->value;

	// don't let it scale below 1ms
	if(frameUsec < 1000)
	{
		frameUsec = 1000;
	}
	sv.timeResidual += usec;

	if ( sv.timeResidual < frameUsec ) {
		// NET_Sleep will give the OS time slices until either get a packet
		// or time enough for a server frame has gone by
		underattack = NET_Sleep( frameUsec - sv.timeResidual );
		return qfalse;
	}

	if(underattack)
		NET_Clear();

	SV_PreFrame( );

	// run the game simulation in chunks
	while ( sv.timeResidual >= frameUsec ) {
		sv.timeResidual -= frameUsec;
		svs.time += frameUsec / 1000;

		// let everything in the world think and move
		G_RunFrame( svs.time );
	}

	// send messages back to the clients
	SV_SendClientMessages();

	Scr_SetLoading(0);

	// update ping based on the all received frames
	SV_CalcPings();

	// check timeouts
	SV_CheckTimeouts();

	// send a heartbeat to the master if needed
	SV_MasterHeartbeat( HEARTBEAT_GAME );

	PbServerProcessEvents();

	// if time is about to hit the 32nd bit, kick all clients
	// and clear sv.time, rather
	// than checking for negative time wraparound everywhere.
	// 2giga-milliseconds = 23 days, so it won't be too often
	if ( svs.time > 0x70000000 ) {
		Q_strncpyz( mapname, sv_mapname->string, sizeof(mapname) );
		SV_Shutdown( "EXE_SERVERRESTARTTIMEWRAP" );
		Com_Restart( );
		// TTimo
		// show_bug.cgi?id=388
		// there won't be a map_restart if you have shut down the server
		// since it doesn't restart a non-running server
		// instead, re-run the current map
		SV_Map( mapname );
		return qtrue;
	}

	// this can happen considerably earlier when lots of clients play and the map doesn't change
	if ( svs.nextSnapshotEntities >= 0x7FFFFFFE - svs.numSnapshotEntities ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15numSnapshotEntities wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

	if ( svs.nextSnapshotClients >= 0x7FFFFFFE - svs.numSnapshotClients ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15numSnapshotClients wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

	if ( svs.nextCachedSnapshotEntities >= 0x7FFFBFFD ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotEntities wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

	if ( svs.nextCachedSnapshotClients >= 0x7FFFEFFD ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotClients wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}


	if ( svs.nextArchivedSnapshotFrames >= 0x7FFFFB4D ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotFrames wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

	if ( svs.nextArchivedSnapshotBuffer >= 0x7DFFFFFD ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotBuffer wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

	if ( svs.nextCachedSnapshotFrames >= 0x7FFFFDFD ) {
		Q_strncpyz( mapname, sv_mapname->string, MAX_QPATH );
		SV_Shutdown( "EXE_SERVERRESTARTMISC\x15svs.nextCachedSnapshotFrames wrapping" );
		Com_Restart( );
		// TTimo see above
		SV_Map( mapname );
		return qtrue;
	}

        if( svs.time > svse.frameNextSecond){	//This runs each second
	    svse.frameNextSecond = svs.time+1000;

	    // the menu kills the server with this cvar
	    if ( sv_killserver->boolean ) {
		SV_Shutdown( "Server was killed.\n" );
		Cvar_SetBool( sv_killserver, qfalse );
		return qtrue;
	    }

	    if(svs.time > svse.frameNextTenSeconds){	//This runs each 10 seconds
		svse.frameNextTenSeconds = svs.time+10000;

		int d, h, m;
		int uptime;

		uptime = Sys_Seconds();
		d = uptime/(60*60*24);
//		uptime = uptime%(60*60*24);
		h = uptime/(60*60);
//		uptime = uptime%(60*60);
		m = uptime/60;

		if(h < 4)
			Cvar_SetString(sv_uptime, va("%i minutes", m+h*60));
		else if(d < 3)
			Cvar_SetString(sv_uptime, va("%i hours", h+d*24));
		else
			Cvar_SetString(sv_uptime, va("%i days", d));

		serverStatus_Write();

	        PHandler_Event(PLUGINS_ONTENSECONDS, NULL);	// Plugin event
/*		if(svs.time > svse.nextsecret){
			svse.nextsecret = svs.time+80000;
			Com_RandomBytes((byte*)&svse.secret,sizeof(int));
		}*/

		if(level.time > level.startTime + 20000){
			for(client = svs.clients, i = 0; i < sv_maxclients->integer; i++, client++){
				if(client->state != CS_ACTIVE)
					continue;
			
				G_PrintRuleForPlayer(client);
				G_PrintAdvertForPlayer(client);
			}
		}
	    }

	}
	return qtrue;
}


void SV_SayToPlayers(int clnum, int team, char* text)
{

	client_t *cl;

	if(clnum >= 0 && clnum < 64){
		SV_SendServerCommand(&svs.clients[clnum], "h \"%s\"", text);
		return;
	}
	if(team == -1)
	{
		SV_SendServerCommand(NULL, "h \"%s\"", text);
		return;
	}
	for(cl = svs.clients, clnum = 0; clnum < sv_maxclients->integer; clnum++){

		if(cl->state < CS_ACTIVE)
			continue;

		if(team != level.clients[clnum].sess.sessionTeam)
			continue;

		SV_SendServerCommand(cl, "h \"%s\"", text);
	}
}

/*
===============
SV_GetUserinfo

===============
*/
void SV_GetUserinfo( int index, char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetUserinfo: bufferSize == %i", bufferSize );
	}
	if ( index < 0 || index >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUserinfo: bad index %i\n", index );
	}
	Q_strncpyz( buffer, svs.clients[ index ].userinfo, bufferSize );
}


qboolean SV_UseUids()
{
    return psvs.useuids;
}

const char* SV_GetMapRotation()
{
    return sv_mapRotation->string;
}

const char* SV_GetNextMap()
{
    return sv_nextmap->string;
}

