/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "q_shared.h"
#include "qcommon_io.h"
#include "cvar.h"
#include "entity.h"
#include "player.h"
#include "plugin_handler.h"
#include "g_shared.h"
#include "g_sv_shared.h"
#include "cmd.h"
#include "server.h"

#include <string.h>
#include <stdarg.h>

/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/

cvar_t *g_speed;
cvar_t *g_disabledefcmdprefix;
cvar_t *g_allowConsoleSay;


__cdecl void ExitLevel( void ) {
	int i;
	gclient_t *gcl;
	client_t *cl;

        PHandler_Event(PLUGINS_ONEXITLEVEL,NULL);

	if(*g_votedMapName->string){
		if(*g_votedGametype->string)
			Cbuf_AddText( EXEC_NOW, va("set g_gametype %s; map %s; set g_votedGametype \"\"; set g_votedMapName \"\"\n", g_votedGametype->string, g_votedMapName->string));
		else
			Cbuf_AddText( EXEC_NOW, va("map %s; set g_votedMapName \"\"\n", g_votedMapName->string));
	}else if(*SV_GetNextMap()){
		Cbuf_AddText( EXEC_NOW, "vstr nextmap\n" );
	}else{
		Cbuf_AddText( EXEC_NOW, "map_rotate\n" );
	}

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		gcl = &level.clients[i];
		cl = &svs.clients[i];
		if ( gcl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		gcl->pers.scoreboard.score = 0;
		// change all client states to connecting, so the early players into the
		// next level will know the others aren't done reconnecting
		if(cl->netchan.remoteAddress.type != NA_BOT)
			gcl->pers.connected = CON_CONNECTING;
	}
	G_LogPrintf( "ExitLevel: executed\n" );
}


int BG_GetPerkIndexForName(const char* name){

	char *perks[] =  {"specialty_gpsjammer", "specialty_bulletaccuracy", "specialty_fastreload", "specialty_rof",
			"specialty_holdbreath", "specialty_bulletpenetration", "specialty_grenadepulldeath",
			"specialty_pistoldeath", "specialty_quieter", "specialty_parabolic", "specialty_longersprint",
			"specialty_detectexplosive", "specialty_explosivedamage", "specialty_exposeenemy",
			"specialty_bulletdamage", "specialty_extraammo", "specialty_twoprimaries",
			"specialty_armorvest", "specialty_fraggrenade", "specialty_specialgrenade", NULL };

        int i;

        if(!name)
            return 20;

        for(i = 0; i < 20; i++){
            if(!Q_stricmp(name, perks[i]))
                break;
        }

        return i;
}


int G_GetSavePersist(void){

	return level.savePersist;

}

void G_SetSavePersist(int val){

	level.savePersist = val;
}

__cdecl void G_RegisterCvarsCallback( ){

    cvar_t** tmp;

    g_speed = Cvar_RegisterInt("g_speed", 190, 1, 6000, 0, "Player's global movement speed is set here");
    g_disabledefcmdprefix = Cvar_RegisterBool("g_disabledefcmdprefix", qtrue, 0, "Disable the interpretation of the !-sign as command");
    g_allowConsoleSay = Cvar_RegisterBool("g_allowConsoleSay", qtrue, CVAR_ARCHIVE, "Flag whether to allow chat from ingame console");
    //g_maxclients
    tmp = (cvar_t**)(0x84bcfe8);
    *tmp = Cvar_RegisterInt("sv_maxclients", 32, 1, 64, CVAR_ARCHIVE,"The maximum number of clients that can connect to a server");

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
__cdecl void QDECL G_LogPrintf( const char *fmt, ... ) {

	va_list argptr;

	char string[1024];
	int stringlen;
	int min, tens, sec;
	int timelen;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

	timelen = strlen(string);

	va_start( argptr, fmt );
	Q_vsnprintf( string + timelen, sizeof( string ) - timelen, fmt, argptr );

	va_end( argptr );

	stringlen = strlen( string );

	G_PrintRedirect(string, stringlen);

	if ( !level.logFile ) {
		return;
	}

	FS_Write( string, stringlen, level.logFile );
}

#define MAX_REDIRECTDESTINATIONS 4

static void (*rd_destinations[MAX_REDIRECTDESTINATIONS])( char *buffer, int len );

void G_PrintRedirect(char* msg, int len)
{

    int i;

    for(i = 0; i < MAX_REDIRECTDESTINATIONS; i++)
    {
        if(rd_destinations[i] == NULL)
            return;

        rd_destinations[i](msg, len);

    }

}
/*
    To Add:
    HL2Rcon_SourceRconSendGameLog(string, stringlen);
*/


void G_PrintAddRedirect(void (*rd_dest)( char *, int))
{
    int i;

    for(i = 0; i < MAX_REDIRECTDESTINATIONS; i++)
    {
        if(rd_destinations[i] == rd_dest)
        {
            Com_Error(ERR_FATAL, "G_PrintAddRedirect: Attempt to add an already defined redirect function twice.");
            return;
        }

        if(rd_destinations[i] == NULL)
        {
            rd_destinations[i] = rd_dest;
            return;
        }
    }
    Com_Error(ERR_FATAL, "G_PrintAddRedirect: Out of redirect handles. Increase MAX_REDIRECTDESTINATIONS to add more redirect destinations");
}
