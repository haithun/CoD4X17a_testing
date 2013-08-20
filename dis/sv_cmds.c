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
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================

===============================================================================

OPERATOR CONSOLE ONLY COMMANDS

These commands can only be entered from stdin or by a remote operator datagram
===============================================================================
*/


typedef enum {
    SAY_CHAT,
    SAY_CONSOLE,
    SAY_SCREEN
} consaytype_t;

/*
==================
SV_ConSay
==================
*/
static void SV_ConSay(client_t *cl, consaytype_t contype) {
	char	*p;
	char	text[1024];
	char	cmd_argbuf[MAX_STRING_CHARS];

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( Cmd_Argc () < 2 ) {
		Com_Printf( "Usage: command text... \n" );
		return;
	}
	*text = 0;
	if(cl){
	    if(contype == SAY_CHAT){
		strcpy (text, sv_contellname->string);
	    }else{
		strcpy (text, "^5PM: ^7");
	    }
	    p = Cmd_Argsv(2, cmd_argbuf, sizeof(cmd_argbuf));
	}else{
	    if(contype == SAY_CHAT){
		strcpy (text, sv_consayname->string);
	    }
	    p = Cmd_Args(cmd_argbuf, sizeof(cmd_argbuf));
	}
	if ( *p == '"' ) {
		p++;
		p[strlen(p)-1] = 0;
	}

	strcat(text, p);

	switch(contype){

	case SAY_CHAT:
		SV_SendServerCommand(cl, "h \"%s\"", text);
	break;
	case SAY_CONSOLE:
		SV_SendServerCommand(cl, "e \"%s\"", text);
	break;
	case SAY_SCREEN:
		SV_SendServerCommand(cl, "c \"%s\"", text);
	break;
	}
}


/*
==================
SV_GetPlayerByHandle

Returns the player with player id or name from Cmd_Argv(1)
==================
*/
typedef struct{
	int uid;
	client_t *cl;
}clanduid_t;




static clanduid_t SV_GetPlayerByHandle( void ) {
	clanduid_t	cl;
	client_t	*lastfound;
	int		i, playermatches;
	char		*s;
	char		cleanName[64];

	cl.uid = 0;
	cl.cl = NULL;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		return cl;
	}

	if ( Cmd_Argc() < 2 ) {
		Com_Printf( "No player specified.\n" );
		return cl;
	}

	s = Cmd_Argv(1);


        if((s[0] == '@' || s[0] == 'u') && isNumeric(&s[1], 0)){

		cl.uid = atoi(&s[1]);
		if(cl.uid < 1){
			cl.uid = 0;
			Com_Printf("Invalid UID specified.\n");
			return cl;
		}

        }else{

        //    cl = SV_GetPlayerByHandle();

		cl.cl = NULL;

		// Check whether this is a numeric player handle
		for(i = 0; s[i] >= '0' && s[i] <= '9'; i++);

		if(!s[i])
		{
			int plid = atoi(s);

			// Check for numeric playerid match
			if(plid >= 0 && plid < sv_maxclients->integer)
			{
				cl.cl = &svs.clients[plid];

				if(!cl.cl->state)
					cl.cl = NULL;
			}
		}
		if(!cl.cl){

			if(strlen(s) < 3){ //Don't process too short names
				Com_Printf( "Player %s is not on the server\n", s );
				cl.cl = NULL;
				return cl;
			}

			playermatches = 0; //This must be one
			lastfound = NULL;

			// check for a exact name match
			for ( i=0, cl.cl=svs.clients ; i < sv_maxclients->integer ; i++, cl.cl++ ) {
				if ( !cl.cl->state ) {
					continue;
				}
				if ( !Q_stricmp( cl.cl->name, s ) ) {
					lastfound = cl.cl;
					playermatches++;
					continue;
				}

				Q_strncpyz( cleanName, cl.cl->name, sizeof(cleanName) );
				Q_CleanStr( cleanName );
				if ( !Q_stricmp( cleanName, s ) ) {
					lastfound = cl.cl;
					playermatches++;
					continue;
				}
			}

			if(!lastfound){ //No exact playermatch found - Now search for partial name matches
				for ( i=0, cl.cl=svs.clients ; i < sv_maxclients->integer ; i++, cl.cl++ ) {
					if ( !cl.cl->state ) {
						continue;
					}
					if ( Q_stristr( cl.cl->name, s ) ) {
						lastfound = cl.cl;
						playermatches++;
						continue;
					}

					Q_strncpyz( cleanName, cl.cl->name, sizeof(cleanName) );
					Q_CleanStr( cleanName );
					if ( Q_stristr( cleanName, s ) ) {
						lastfound = cl.cl;
						playermatches++;
						continue;
					}
				}
			}

			if(!lastfound){ //Still nothing found!
				Com_Printf( "Player %s is not on the server\n", s );
				cl.cl = NULL;
				return cl;
			}else if(playermatches != 1){
				Com_Printf( "Dup player matches!\n", s );
				cl.cl = NULL;
				return cl;
			}else{
				cl.cl = lastfound;
			}
		}
                cl.uid = cl.cl->uid;
        }

        if(!cl.cl && cl.uid > 0){ //See whether this player is currently onto server
            for(i = 0, cl.cl=svs.clients; i < sv_maxclients->integer; i++, cl.cl++){
                if(cl.cl->state && cl.uid == cl.cl->uid){
                    break;
                }
            }
            if(i == sv_maxclients->integer)
                cl.cl = NULL;
        }

	return cl;
}

/*
==================
SV_GetPlayerByNum

Returns the player with idnum from Cmd_Argv(1)
==================
*/
static client_t *SV_GetPlayerByNum( void ) {
	client_t	*cl;
	int			i;
	int			idnum;
	char		*s;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		return NULL;
	}

	if ( Cmd_Argc() < 2 ) {
		Com_Printf( "No player specified.\n" );
		return NULL;
	}

	s = Cmd_Argv(1);

	for (i = 0; s[i]; i++) {
		if (s[i] < '0' || s[i] > '9') {
			Com_Printf( "Bad slot number: %s\n", s);
			return NULL;
		}
	}
	idnum = atoi( s );
	if ( idnum < 0 || idnum >= sv_maxclients->integer ) {
		Com_Printf( "Bad client slot: %i\n", idnum );
		return NULL;
	}

	cl = &svs.clients[idnum];
	if ( !cl->state ) {
		Com_Printf( "Client %i is not active\n", idnum );
		return NULL;
	}
	return cl;
}


/*
==================
SV_ConSayChat_f
==================
*/

static void SV_ConSayChat_f(void) {
    SV_ConSay(NULL,SAY_CHAT);
}

/*
==================
SV_ConSayConsole_f
==================
*/

static void SV_ConSayConsole_f(void) {
    SV_ConSay(NULL,SAY_CONSOLE);
}

/*
==================
SV_ConSayScreen_f
==================
*/

static void SV_ConSayScreen_f(void) {
    SV_ConSay(NULL,SAY_SCREEN);
}

/*
==================
SV_ConTell
==================
*/

static void SV_ConTell( consaytype_t contype) {

    client_t *cl;

    if ( Cmd_Argc() < 3 ) {
	Com_Printf( "1. Usage: tellcommand clientnumber text... \n2. Usage: tellcommand \"client by name\" text...\n" );
	return;
    }
    cl = SV_GetPlayerByHandle().cl;

    if(cl != NULL){
        SV_ConSay(cl,contype);
    }
}

/*
==================
SV_ConTellScreen_f
==================
*/
static void SV_ConTellScreen_f(void) {
    SV_ConTell(SAY_SCREEN);
}

/*
==================
SV_ConTellConsole_f
==================
*/
static void SV_ConTellConsole_f(void) {
    SV_ConTell(SAY_CONSOLE);
}

/*
==================
SV_ConTellChat_f
==================
*/
static void SV_ConTellChat_f(void) {
    SV_ConTell(SAY_CHAT);
}


/*
===========
SV_DumpUser_f

Examine all a users info strings FIXME: move to game
===========
*/

#define MAX_NICKNAMES 6

static void SV_DumpUser_f( void ) {
	clanduid_t	cl;
	char		*guid;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( Cmd_Argc() != 2 ) {
		Com_Printf ("Usage: dumpuser <userid>\n");
		return;
	}

	guid = SV_IsGUID(Cmd_Argv(1));
	if(guid){
		cl.cl = NULL;
		cl.uid = 0;
	}else{
		cl = SV_GetPlayerByHandle();
		if ( !cl.uid && !cl.cl ) {
			return;
		}
	}

	Com_Printf( "\nuserinfo\n" );
	Com_Printf( "----------------------------------------------------\n" );

	if(cl.cl){

		Info_Print( cl.cl->userinfo );

		Com_Printf ("pbguid               %s\n", cl.cl->pbguid );

		switch(cl.cl->authentication){
		    case 1:
			Com_Printf ("authentication       %s\n", AUTHORIZE_SERVER_NAME);
		    break;
		    case 0:
			Com_Printf ("authentication       %s timed out\n", AUTHORIZE_SERVER_NAME);
		    break;
		    case -1:
			Com_Printf ("authentication       Plugin or N/A\n");
		    break;
		    default:
			Com_Printf ("authentication       unknown\n");
		}
		if(cl.cl->OS == 'M'){
			Com_Printf ("OperatingSystem      Mac OS X\n");
		}else if(cl.cl->OS == 'W'){
			Com_Printf ("OperatingSystem      Windows\n");
		}else{
			Com_Printf ("OperatingSystem      Unknown\n");
		}
		if(cl.cl->uid > 0){
			Com_Printf ("PlayerUID            %i\n",cl.cl->uid);
		}else{
			Com_Printf ("PlayerUID            N/A\n");
		}

		if(cl.cl->uid <= 0)
			return;
        } else {
		Com_Printf("Player is not on server.\n");
        }

//	InsertPluginEvent
/*
	if(psvs.useuids != 1)
		return;


	Com_Printf( "\nExtended info\n" );
	Com_Printf( "----------------------------------------------------\n" );

	if(cl.uid || guid){

		*infostring = 0;
		if(cl.uid)
			Info_SetValueForKey( infostring, "UID", va("%i",cl.uid));

		else
			Info_SetValueForKey( infostring, "GUID", guid);

		if(svse.cmdInvoker.currentCmdInvoker)
			CL_AddReliableCommand(&svse.authserver, va("getUserinfo Ticket %i \"%s\"", svs.clients[svse.cmdInvoker.clientnum].challenge+QT_CMDQUERY, infostring));
		else
			CL_AddReliableCommand(&svse.authserver, va("getUserinfo Ticket %i \"%s\"", -1, infostring));

		Com_DPrintf("Query sent:\n");

	}else{
		Com_Printf("N/A\n");
	}*/
}

/*
================
SV_Status_f
================
*/
static void SV_Status_f( void ) {
	int			i, j, l;
	client_t	*cl;
	gclient_t	*gclient;
	const char		*s;
	int			ping;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		Com_Printf( "Server is not running.\n" );
		return;
	}

	Com_Printf ("map: %s\n", sv_mapname->string );

	Com_Printf ("num score ping guid                             name            lastmsg address               qport rate\n");
	Com_Printf ("--- ----- ---- -------------------------------- --------------- ------- --------------------- ----- -----\n");

	for (i=0,cl=svs.clients, gclient = level.clients; i < sv_maxclients->integer ; i++, cl++, gclient++)
	{
		if (!cl->state)
			continue;
		Com_Printf ("%3i ", i);
		Com_Printf ("%5i ", gclient->pers.scoreboard.score);
		if (cl->state == CS_CONNECTED)
			Com_Printf ("CNCT ");
		else if (cl->state == CS_ZOMBIE)
			Com_Printf ("ZMBI ");
		else if (cl->state == CS_PRIMED)
			Com_Printf ("PRIM ");
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			Com_Printf ("%4i ", ping);
		}

		Com_Printf ("%s", cl->pbguid );

		l = 33 - strlen(cl->pbguid);
		j = 0;

		do
		{
			Com_Printf (" ");
			j++;
		} while(j < l);

		Com_Printf ("%s", cl->shortname);

		// TTimo adding a ^7 to reset the color
		// NOTE: colored names in status breaks the padding (WONTFIX)
		Com_Printf ("^7");
		l = 16 - Q_PrintStrlen(cl->shortname);
		j = 0;

		do
		{
			Com_Printf (" ");
			j++;
		} while(j < l);

		Com_Printf ("%7i ", svs.time - cl->lastPacketTime );

		s = NET_AdrToString( &cl->netchan.remoteAddress );
		Com_Printf ("%s", s);
		l = 21 - strlen(s);
		j = 0;

		do
		{
			Com_Printf(" ");
			j++;
		} while(j < l);

		Com_Printf (" %5i", cl->netchan.qport);

		Com_Printf (" %5i", cl->rate);

		Com_Printf ("\n");
	}
	Com_Printf ("\n");
}

void QDECL Cmd_PrintAdministrativeLog( const char *fmt, ... ) {

	Sys_EnterCriticalSection(5);

	va_list		argptr;
	char		msg[MAXPRINTMSG];
	char		inputmsg[MAXPRINTMSG];
	struct tm 	*newtime;
	char*		ltime;

        // logfile
	if ( com_logfile && com_logfile->integer && !loadconfigfiles) {
        // TTimo: only open the qconsole.log if the filesystem is in an initialized state
        //   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)

	    va_start (argptr,fmt);
	    Q_vsnprintf (inputmsg, sizeof(inputmsg), fmt, argptr);
	    va_end (argptr);

	    newtime = localtime( &realtime );
	    ltime = asctime( newtime );
	    ltime[strlen(ltime)-1] = 0;

	    if ( !adminlogfile && FS_Initialized()) {

			adminlogfile = FS_FOpenFileAppend( "adminactions.log" );
			// force it to not buffer so we get valid
			if ( adminlogfile ){
				FS_ForceFlush(adminlogfile);
				FS_Write(va("\nLogfile opened on %s\n\n", ltime), strlen(va("\nLogfile opened on %s\n\n", ltime)), adminlogfile);
			}
	    }

	    if ( adminlogfile && FS_Initialized()) {
		if(psvs.useuids)
			Com_sprintf(msg, sizeof(msg), "%s - Admin %i with %i power %s\n", ltime, svse.cmdInvoker.currentCmdInvoker, svse.cmdInvoker.currentCmdPower, inputmsg);
		else
			Com_sprintf(msg, sizeof(msg), "%s - Admin %s with %i power %s\n", ltime, svse.cmdInvoker.currentCmdInvokerGuid, svse.cmdInvoker.currentCmdPower, inputmsg);

		FS_Write(msg, strlen(msg), adminlogfile);
	    }

	}
	Sys_LeaveCriticalSection(5);
}


/*
============
Cmd_RemoteSetAdmin_f
============
*/

static void Cmd_RemoteSetAdmin_f() {

    adminPower_t *admin;
    adminPower_t *this;
    int power;
    int uid = 0;
    char *guid;
    clanduid_t cl = { 0 };

    power = atoi(Cmd_Argv(2));

    if ( Cmd_Argc() != 3 || power < 1 || power > 100) {
        if(psvs.useuids)
            Com_Printf( "Usage: setAdmin <user (online-playername | online-playerslot | uid @number)> <power ([1,100])>\n" );
        else
            Com_Printf( "Usage: setAdmin <user (online-playername | online-playerslot | guid )> <power ([1,100])>\n" );
        return;
    }

    if(psvs.useuids){

        uid = SV_GetPlayerByHandle().uid;
        if(!uid){
            Com_Printf("No such player\n");
            return;
        }

        NV_ProcessBegin();

        for(admin = psvs.adminpower ; admin ; admin = admin->next){
            if(admin->uid == uid){
                if(admin->power != power){
                    admin->power = power;

                    Com_Printf( "Admin power changed for: uid: %i to level: %i\n", uid, power);
                    Cmd_PrintAdministrativeLog( "changed power of admin with uid: %i to new power: %i", uid, power);
                }
                NV_ProcessEnd();
                return;
            }
        }

        this = Z_Malloc(sizeof(adminPower_t));
        if(this){
            this->uid = uid;
            this->power = power;
            this->next = psvs.adminpower;
            psvs.adminpower = this;
            Com_Printf( "Admin added: uid: %i level: %i\n", uid, power);
            Cmd_PrintAdministrativeLog( "added a new admin with uid: %i and power: %i", uid, power);
        }

    }else{

        guid = SV_IsGUID(Cmd_Argv(1));

        if(!guid)
        {
            cl = SV_GetPlayerByHandle();
            if(!cl.cl){
                Com_Printf("Error: No such player\n");
                return;
            }else{
                if(cl.cl->authentication != 1){
                        Com_PrintError("Bad CD-KEY / Cracked server\n");
                        return;
                }
                guid = &cl.cl->pbguid[24];
            }
        }

        NV_ProcessBegin();

        for(admin = psvs.adminpower ; admin ; admin = admin->next){
            if(!Q_stricmp(admin->guid, guid)){
                if(admin->power != power){
                    admin->power = power;

                    Com_Printf( "Admin power changed for: guid: %s to level: %i\n", guid, power);
                    Cmd_PrintAdministrativeLog( "changed power of admin with guid: %s to new power: %i", guid, power);
                }
                NV_ProcessEnd();
                return;
            }
        }

        this = Z_Malloc(sizeof(adminPower_t));
        if(this){
            Q_strncpyz(this->guid, guid, sizeof(this->guid));
            this->power = power;
            this->next = psvs.adminpower;
            psvs.adminpower = this;
            Com_Printf( "Admin added: guid: %s level: %i\n", guid, power);
            Cmd_PrintAdministrativeLog( "added a new admin with guid: %s and power: %i", guid, power);
        }
    }

    NV_ProcessEnd();
}




/*
============
Cmd_RemoteUnsetAdmin_f
============
*/

static void Cmd_RemoteUnsetAdmin_f() {

    adminPower_t *admin, **this;
    int uid;
    char *guid;
    clanduid_t cl = { 0 };


    if (Cmd_Argc() != 2) {
        if(psvs.useuids)
            Com_Printf( "Usage: unsetAdmin <user (online-playername | online-playerslot | uid @number)>\n" );
        else
            Com_Printf( "Usage: unsetAdmin <user (online-playername | online-playerslot | guid )>\n" );
        return;
    }


    if(psvs.useuids){

        uid = SV_GetPlayerByHandle().uid;
        if(!uid){
            Com_Printf("No such player\n");
            return;
        }


        NV_ProcessBegin();
        for(this = &psvs.adminpower, admin = *this; admin ; admin = *this)
        {

            if(admin->uid == uid){
                *this = admin->next;
                Z_Free(admin);
                NV_ProcessEnd();
                Com_Printf( "User removed: uid: %i\n", uid);
                Cmd_PrintAdministrativeLog( "removed admin with uid: %i", uid);
                return;
            }
            this = &admin->next;
        }
    }else{

        guid = SV_IsGUID(Cmd_Argv(1));

        if(!guid)
        {
            cl = SV_GetPlayerByHandle();
            if(!cl.cl){
                Com_Printf("Error: No such player\n");
                return;
            }else{
                guid = &cl.cl->pbguid[24];
            }
        }

        NV_ProcessBegin();
        for(this = &psvs.adminpower, admin = *this; admin ; admin = *this)
        {

            if(!Q_stricmp(admin->guid, guid)){
                *this = admin->next;
                Z_Free(admin);
                NV_ProcessEnd();
                Com_Printf( "User removed: guid: %s\n", guid);
                Cmd_PrintAdministrativeLog( "removed admin with guid: %s", guid);
                return;
            }
            this = &admin->next;
        }

    }


    Com_Printf( "Error: No such user in database\n");
    NV_ProcessEnd();
}




/*
============
Cmd_RemoteSetPermission_f
Changes minimum-PowerLevel of a command
============
*/

static void Cmd_RemoteSetPermission_f() {

    if ( Cmd_Argc() != 3 || atoi(Cmd_Argv(2)) < 1 || atoi(Cmd_Argv(2)) > 100) {
	Com_Printf( "Usage: setCmdMinPower <command> <minpower ([1,100])>\n" );
	return;
    }
    NV_ProcessBegin();
    Cmd_SetPower(Cmd_Argv(1), atoi(Cmd_Argv(2)));
    Cmd_PrintAdministrativeLog( "changed required power of cmd: %s to new power: %s", Cmd_Argv(1), Cmd_Argv(2));
    NV_ProcessEnd();
}

/*
============
Cmd_RemoteListAdmin_f
============
*/

static void Cmd_RemoteListAdmin_f() {

    adminPower_t *admin;
    int i;

    for(i = 0, admin = psvs.adminpower ; admin ; i++, admin = admin->next){
                Com_Printf( "Admin : uid: %i to: level: %i\n", admin->uid, admin->power);
    }
    Com_Printf( "%i registered Admins\n", i);
}


/*
============
Cmd_ListPower_f
============
*/

static void Cmd_ListPower_f() {

	cmd_function_t  *cmd;
	int i, hidden, j, l;
	char            *match;

	if ( Cmd_Argc() > 1 ) {
		match = Cmd_Argv( 1 );
	} else {
		match = NULL;
	}

	i = 0;
	hidden = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( (match && !Com_Filter( match, cmd->name, qfalse ))) {
			continue;
		}
		if(cmd->minPower == 100){
			hidden++;
			continue;
		}
		Com_Printf ("%s", cmd->name );

		l = 24 - strlen(cmd->name);
		j = 0;

		do
		{
			Com_Printf (" ");
			j++;
		} while(j < l);

		Com_Printf( "%d\n", cmd->minPower );
		i++;
	}
	Com_Printf( "\n%i commands with specified power settings are shown\n", i );
	Com_Printf( "%i commands are hidden because the required power level for those commands is set to 100\n", hidden );
	Com_Printf( "Type cmdlist to get a complete list of all commands\n");
}



/*
============
Cmd_List_f
============
*/
__cdecl void Cmd_List_f( void ) {
	cmd_function_t  *cmd;
	int i;
	char            *match;

	if ( Cmd_Argc() > 1 ) {
		match = Cmd_Argv( 1 );
	} else {
		match = NULL;
	}

	i = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( (match && !Com_Filter( match, cmd->name, qfalse )) || svse.cmdInvoker.currentCmdPower < cmd->minPower) {
			continue;
		}
		Com_Printf( "%s\n", cmd->name );
		i++;
	}
	Com_Printf( "%i commands\n", i );
}


/*
============
Cmd_UnbanPlayer_f
============
*/

static void Cmd_UnbanPlayer_f() {

    char* guid;
    int uid;
    qboolean unbanstatus;
    ipBanList_t *thisipban;
    int i;


    if ( Cmd_Argc() < 2) {
        if(psvs.useuids)
            Com_Printf( "Usage: unban <@uid>\n" );
        else
            Com_Printf( "Usage: unban <guid>\n" );
        return;
    }

    guid = SV_IsGUID(Cmd_Argv(1));
    if(guid){
        uid = 0;
        unbanstatus = SV_RemoveBan(uid, guid, NULL);
    }else{
        uid = SV_GetPlayerByHandle().uid;
        if(uid < 1){
            Com_Printf("Error: This player can not be unbanned, no such player\n");
            return;
        }
        unbanstatus = SV_RemoveBan(uid, NULL, NULL);
/*
        if(sv_authorizemode->integer == 1)
            SV_SendBroadcastUnban(uid, svse.cmdInvoker.currentCmdInvoker);
*/
    }

    //InsertPluginEvent


    if(!unbanstatus){
        Com_Printf("Error: Tried to unban a player who is not actually banned\n");
        return;
    }else{

        if(uid)
            Cmd_PrintAdministrativeLog( "Removed ban for player with uid: %i", uid);
        else
            Cmd_PrintAdministrativeLog( "Removed ban for player with guid: %s", guid);
    }

    for(thisipban = &svse.ipBans[0], i = 0; i < 1024; thisipban++, i++){
        if(uid == thisipban->uid){
            Com_Memset(thisipban,0,sizeof(ipBanList_t));
        }
    }
}



/*
============
Cmd_BanPlayer_f
============
*/

static void Cmd_BanPlayer_f() {

    int i;
    char* guid = NULL;
    clanduid_t cl = { 0 };
    char banreason[256];

    if ( Cmd_Argc() < 3) {
        if(Q_stricmp(Cmd_Argv(0), "banUser") || Q_stricmp(Cmd_Argv(0), "banClient")){
            if(Cmd_Argc() < 2){
                if(psvs.useuids)
                    Com_Printf( "Usage: banUser <online-playername | online-playerslot | @uid>\n" );
                else
                    Com_Printf( "Usage: banUser <online-playername | online-playerslot | guid>\n" );

                return;
            }

        }else{
            if(psvs.useuids)
                Com_Printf( "Usage: permban <online-playername | online-playerslot | @uid> <Reason for this ban (max 126 chars)>\n" );
            else
                Com_Printf( "Usage: permban <online-playername | online-playerslot | guid> <Reason for this ban (max 126 chars)>\n" );

            return;
        }
    }



    if(!psvs.useuids){

        guid = SV_IsGUID(Cmd_Argv(1));

        if(!guid)
        {
            cl = SV_GetPlayerByHandle();
            if(!cl.cl){
                Com_Printf("Error: This player can not be banned, no such player\n");
                return;
            }else{
                guid = &cl.cl->pbguid[24];
            }
        }

    }else{
        cl = SV_GetPlayerByHandle();
        if(!cl.uid){
            //Try to ban GUID
            guid = SV_IsGUID(Cmd_Argv(1));
            if(!guid){
                if(!cl.cl){
                    Com_Printf("Error: This player can not be banned, no such player\n");
                    return;
                }else{
                    if(strlen(cl.cl->pbguid) == 32){
                        guid = &cl.cl->pbguid[24];

                    }else{
                            Com_Printf("Error: This player can not be banned, no such player\n");
                            return;
                    }
                }
            }
        }
    }


    banreason[0] = 0;
    if ( Cmd_Argc() > 2) {
        for(i = 2; Cmd_Argc() > i ;i++){
            Q_strcat(banreason,256,Cmd_Argv(i));
            Q_strcat(banreason,256," ");
        }
    }else{
        Q_strncpyz(banreason, "The admin has no reason given", 256);
    }

    if(strlen(banreason) > 126){
        Com_Printf("Error: You have exceeded the maximum allowed length of 126 for the reason\n");
        return;
    }

        if(cl.cl){
            //Banning
            SV_AddBan(cl.uid, svse.cmdInvoker.currentCmdInvoker, guid, cl.cl->name, (time_t)-1, banreason);
/*            if(cl.uid > 0 && !Q_stricmp(Cmd_Argv(0), "bpermban") && sv_authorizemode->integer == 1){
                SV_SendBroadcastPermban(cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason, cl.cl->name);
            }*/

            //Messages and kick
            if(psvs.useuids){
                Com_Printf( "Banrecord added for player: %s uid: %i\n", cl.cl->name, cl.uid);
                Cmd_PrintAdministrativeLog( "banned player: %s uid: %i with the following reason: %s", cl.cl->name, cl.uid, banreason);
                SV_DropClient(cl.cl, va("You have got a permanent ban onto this gameserver\nYour ban will %s expire\nYour UID is: %i    Banning admin UID is: %i\nReason for this ban:\n%s",
                    "never", cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason));

            }else{
                Com_Printf( "Banrecord added for player: %s guid: %s\n", cl.cl->name, cl.cl->pbguid);
                Cmd_PrintAdministrativeLog( "banned player: %s guid: %s with the following reason: %s", cl.cl->name, cl.cl->pbguid, banreason);
                SV_DropClient(cl.cl, va("You have got a permanent ban onto this gameserver\nYour GUID is: %s    Banning admin GUID is: %s\nReason for this ban:\n%s",
                    cl.cl->pbguid, svse.cmdInvoker.currentCmdInvokerGuid, banreason));
            }

        }else{
            //Banning
            SV_AddBan(cl.uid, svse.cmdInvoker.currentCmdInvoker, guid, "N/A", (time_t)-1, banreason);
/*
            if(cl.uid > 0 && !Q_stricmp(Cmd_Argv(0), "bpermban") && sv_authorizemode->integer == 1){
                SV_SendBroadcastPermban(cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason, "N/A");
            }
*/
            //Messages
            if(psvs.useuids){
                Com_Printf( "Banrecord added for uid: %i\n", cl.uid);
                Cmd_PrintAdministrativeLog( "banned player uid: %i with the following reason: %s", cl.uid, banreason);
            }else{
                Com_Printf( "Banrecord added for guid: %s\n", guid);
                Cmd_PrintAdministrativeLog( "banned player guid: %s with the following reason: %s", guid, banreason);
            }
        }
        //InsertPluginEvent
}


/*
============
Cmd_TempBanPlayer_f
============
*/

static void Cmd_TempBanPlayer_f() {

    ipBanList_t *thisipban;
    int i;
    clanduid_t cl = { 0 };
    char banreason[256];
    int duration = 0;
    char endtime[32];
    char* temp;
    time_t aclock;
    time(&aclock);
    int length;
    char buff[8];
    char *guid = NULL;

    if ( Cmd_Argc() < 4) {

        if(psvs.useuids)
            Com_Printf( "Usage: tempban <online-playername | online-playerslot | @uid> <\"m\"inutes | \"h\"ours | \"d\"ays (example1: 5h   example2: 40m)>  <Reason for this ban (max 126 chars)>\n" );
        else
            Com_Printf( "Usage: tempban <online-playername | online-playerslot | guid> <\"m\"inutes | \"h\"ours | \"d\"ays (example1: 5h   example2: 40m)>  <Reason for this ban (max 126 chars)>\n" );
        return;
    }


    if(!psvs.useuids){

        guid = SV_IsGUID(Cmd_Argv(1));

        if(!guid)
        {
            cl = SV_GetPlayerByHandle();
            if(!cl.cl){
                Com_Printf("Error: This player can not be banned, no such player\n");
                return;
            }else{
                guid = &cl.cl->pbguid[24];
            }
        }

    }else{
        cl = SV_GetPlayerByHandle();
        if(!cl.uid){
            //Try to ban GUID
            guid = SV_IsGUID(Cmd_Argv(1));
            if(!guid){
                if(!cl.cl){
                    Com_Printf("Error: This player can not be banned, no such player\n");
                    return;
                }else{
                    if(strlen(cl.cl->pbguid) == 32){
                        guid = &cl.cl->pbguid[24];

                    }else{
                            Com_Printf("Error: This player can not be banned, no such player\n");
                            return;
                    }
                }
            }
        }
    }


    length = strlen(Cmd_Argv(2));
    if(length > 7){
        Com_Printf("Error: Did not got a valid bantime\n");
        return;
    }


    if(Cmd_Argv(2)[length-1] == 'm'){
        if(isNumeric(Cmd_Argv(2),length-1)){
            Q_strncpyz(buff,Cmd_Argv(2),length);
            duration = atoi(buff);
        }

    }else if(Cmd_Argv(2)[length-1] == 'h'){
        if(isNumeric(Cmd_Argv(2),length-1)){
            Q_strncpyz(buff,Cmd_Argv(2),length);
            duration = (atoi(buff) * 60);
        }
    }else if(Cmd_Argv(2)[length-1] == 'd'){
        if(isNumeric(Cmd_Argv(2),length-1)){
            Q_strncpyz(buff,Cmd_Argv(2),length);
            duration = (atoi(buff) * 24 * 60);
        }
    }
    if(duration < 1){
        Com_Printf("Error: Did not got a valid bantime\n");
        return;
    }
    if(duration > 60*24*30){
        Com_Printf("Error: Can not issue a temporarely ban that last longer than 30 days\n");
        return;
    }

    banreason[0] = 0;
    for(i = 3; Cmd_Argc() > i ;i++){
        Q_strcat(banreason,256,Cmd_Argv(i));
        Q_strcat(banreason,256," ");
    }
    if(strlen(banreason) > 126){
        Com_Printf("Error: You have exceeded the maximum allowed length of 126 for the reason\n");
        return;
    }


    if(psvs.useuids){//Just for the case this ban has got shortened more then the ip-banduration
            for(thisipban = &svse.ipBans[0], i = 0; i < 1024; thisipban++, i++){
                if(cl.uid == thisipban->uid){
                    Com_Memset(thisipban,0,sizeof(ipBanList_t));
                }
            }
    }

        time_t expire = (aclock+(time_t)(duration*60));
        temp = ctime(&expire);
        temp[strlen(temp)-1] = 0;
        Q_strncpyz(endtime, temp, sizeof(endtime));

        if(cl.cl){

            SV_AddBan(cl.uid, svse.cmdInvoker.currentCmdInvoker, guid, cl.cl->name, expire, banreason);
/*            if(cl.uid  > 0 && !Q_stricmp(Cmd_Argv(0), "btempban") && sv_authorizemode->integer == 1){
                SV_SendBroadcastTempban(cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason, Cmd_Argv(2), cl.cl->name);
            }
*/
            if(psvs.useuids && cl.uid > 0){

                Com_Printf( "Banrecord added for player: %s uid: %i\n", cl.cl->name, cl.uid);
                Cmd_PrintAdministrativeLog( "temporarily banned player: %s uid: %i until %s with the following reason: %s", cl.cl->name, cl.uid, endtime, banreason);
                SV_DropClient(cl.cl, va("You have got a temporarily ban onto this gameserver\nYour ban will expire on: %s UTC\nYour UID is: %i    Banning admin UID is: %i\nReason for this ban:\n%s",
                    endtime, cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason));

            }else{
                Com_Printf( "Banrecord added for player: %s guid: %s\n", cl.cl->name, cl.cl->pbguid);
                Cmd_PrintAdministrativeLog( "temporarily banned player: %s guid: %s until %s with the following reason: %s", cl.cl->name, cl.cl->pbguid, endtime, banreason);
                SV_DropClient(cl.cl, va("You have got a temporarily ban onto this gameserver\nYour ban will expire on: %s UTC\nYour GUID is: %s    Banning admin UID is: %s\nReason for this ban:\n%s",
                    endtime, cl.cl->pbguid, svse.cmdInvoker.currentCmdInvokerGuid, banreason));
            }

        }else{

            SV_AddBan(cl.uid, svse.cmdInvoker.currentCmdInvoker, guid, "N/A", expire, banreason);
/*
            if(cl.uid  > 0 && !Q_stricmp(Cmd_Argv(0), "btempban") && sv_authorizemode->integer == 1){
                SV_SendBroadcastTempban(cl.uid, svse.cmdInvoker.currentCmdInvoker, banreason, Cmd_Argv(2), "N/A");
            }
*/
            if(psvs.useuids && cl.uid > 0){
                Com_Printf( "Banrecord added for uid: %i\n", cl.uid);
                Cmd_PrintAdministrativeLog( "temporarily banned player uid: %i until %s with the following reason: %s", cl.uid, endtime, banreason);

            }else{
                Com_Printf( "Banrecord added for guid: %s\n", guid);
                Cmd_PrintAdministrativeLog( "temporarily banned player guid: %s until %s with the following reason: %s", guid, endtime, banreason);
            }
        }
        //InsertPluginEvent
}






/*
============
Cmd_KickPlayer_f
============
*/

static void Cmd_KickPlayer_f() {

    int i;
    clanduid_t cl;
    char kickreason[256];

    if ( Cmd_Argc() < 2) {
        Com_Printf( "Usage: kick < user (online-playername | online-playerslot | uid (@#  or  u#)) > <Reason for this kick (max 126 chars) (optional)>\n" );
        return;
    }

    cl = SV_GetPlayerByHandle();
    if(!cl.cl){
            Com_Printf("Error: This player is not online and can not be kicked\n");
            return;
    }

    kickreason[0] = 0;
    if ( Cmd_Argc() > 2) {
        for(i = 2; Cmd_Argc() > i ;i++){
            Q_strcat(kickreason,256,Cmd_Argv(i));
            Q_strcat(kickreason,256," ");
        }
    }else{
        Q_strncpyz(kickreason, "The admin has no reason given", 256);
    }
    if(strlen(kickreason) >= 256 ){
        Com_Printf("Error: You have exceeded the maximum allowed length of 126 for the reason\n");
        return;
    }
    if(psvs.useuids){
        if(cl.uid){
            Com_Printf( "Player kicked: %s ^7uid: %i\nReason: %s\n", cl.cl->name, cl.uid, kickreason);
            Cmd_PrintAdministrativeLog( "kicked player: %s uid: %i with the following reason: %s", cl.cl->name, cl.uid, kickreason);
        }else{
            Com_Printf( "Player kicked: %s ^7uid: N/A\nReason: %s\n", cl.cl->name, kickreason);
            Cmd_PrintAdministrativeLog( "kicked player: %s unknown uid with the following reason: %s", cl.cl->name, kickreason);
        }

        SV_DropClient(cl.cl, va("Player kicked:\nAdmin UID is: %i\nReason for this kick:\n%s",
         svse.cmdInvoker.currentCmdInvoker, kickreason));

    }else{

        Com_Printf( "Player kicked: %s ^7guid: %s\nReason: %s\n", cl.cl->name, cl.cl->pbguid, kickreason);
        Cmd_PrintAdministrativeLog( "kicked player: %s guid: %s with the following reason: %s", cl.cl->name, cl.cl->pbguid, kickreason);

        SV_DropClient(cl.cl, va("Player kicked:\nAdmin GUID is: %s\nReason for this kick:\n%s",
         svse.cmdInvoker.currentCmdInvokerGuid, kickreason));
    }

}

/*
================
SV_DumpBanlist_f
================
*/

void SV_DumpBanlist_f(){
    SV_DumpBanlist();
}



/*
================
SV_MiniStatus_f
================
*/
static void SV_MiniStatus_f( void ) {
	int			i, j, l;
	client_t	*cl;
	gclient_t	*gclient;
	const char	*s;
	int		ping;
	qboolean	odd = qfalse;

	// make sure server is running
	if ( !com_sv_running->boolean ) {
		Com_Printf( "Server is not running.\n" );
		return;
	}

	Com_Printf ("map: %s\n", sv_mapname->string );

	Com_Printf ("num score ping uid       name                             address                                        FPS OS  authstate(debug) \n");

	Com_Printf ("--- ----- ---- --------- -------------------------------- ---------------------------------------------- --- --- ----------------\n");
	for (i=0,cl=svs.clients, gclient = level.clients ; i < sv_maxclients->integer ; i++, cl++, gclient++)
	{
		if (!cl->state)
			continue;

		if(odd)
			Com_Printf ("^8");
		else
			Com_Printf ("^7");

		Com_Printf ("%3i ", i);
		Com_Printf ("%5i ", gclient->pers.scoreboard.score);
		if (cl->state == CS_CONNECTED)
			Com_Printf ("CNCT ");
		else if (cl->state == CS_ZOMBIE)
			Com_Printf ("ZMBI ");
		else if (cl->state == CS_PRIMED)
			Com_Printf ("PRIM ");
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			Com_Printf ("%4i ", ping);
		}


		if(cl->uid > 0){
			Com_Printf ("%9i ", cl->uid );
		}else{
			Com_Printf ("      N/A ");
		}

		Com_Printf ("%s", cl->name);

		// TTimo adding a ^7 to reset the color
		// NOTE: colored names in status breaks the padding (WONTFIX)

		if(odd)
			Com_Printf ("^8");
		else
			Com_Printf ("^7");

		l = 33 - Q_PrintStrlen(cl->name);
		j = 0;

		do
		{
			Com_Printf (" ");
			j++;
		} while(j < l);

		s = NET_AdrToConnectionStringMask( &cl->netchan.remoteAddress );
		Com_Printf ("%s", s);
		l = 47 - strlen(s);
		j = 0;

		do
		{
			Com_Printf(" ");
			j++;
		} while(j < l);

		Com_Printf("%3i ", cl->clFPS);

		switch(cl->OS){
			case 'M':
				Com_Printf ("Mac ");
				break;
			case 'W':
				Com_Printf ("Win ");
				break;
			default:
				Com_Printf ("N/A ");
		}
		switch(cl->authstate){
			case CAU_NEEDHWINFO:
				Com_Printf ("Need HW-Info");
				break;
			case CAU_GOTGUIDHWINFO:
				Com_Printf ("Got Guid and HW-Info");
				break;
			case CAU_REQUESTEDUID:
				Com_Printf ("Awaiting UID");
				break;		//Do nothing, just wait
			case CAU_GOTUID:
				Com_Printf ("Got UID");
				break;
			case CAU_REQUESTEDUSERINFO:
				Com_Printf ("Awaiting Userinfo");
				break;		//Do nothing, just wait
			case CAU_GOTUSERINFO:
				Com_Printf ("Got Userinfo");
				break;
			case CAU_GUIDUPDATED:
				Com_Printf ("GUID Updated");
				break;
			case CAU_FINISHED:
				Com_Printf ("Done");
				break;
			default:
				Com_Printf ("?");
		}
		odd = ~odd;
		Com_Printf ("\n");
	}
}





/*
==================
SV_Heartbeat_f

Also called by SV_DropClient, SV_DirectConnect, and SV_SpawnServer
==================
*/
void SV_Heartbeat_f( void ) {
	svse.nextHeartbeatTime = 0;
}


/*
============
Cmd_ExecuteTranslatedCommand_f
============
*/

static void Cmd_ExecuteTranslatedCommand_f(){

    int i;
    char outstr[128];

    char *cmdname = Cmd_Argv(0);
    char *cmdstring = NULL;
    char *tmp;

    for(i=0; i < MAX_TRANSCMDS; i++){
        if(!Q_stricmp(cmdname, psvs.translatedCmd[i].cmdname)){
            cmdname = psvs.translatedCmd[i].cmdname;
            cmdstring = psvs.translatedCmd[i].cmdargument;
            break;
        }
    }
    if(!cmdstring) return;

    tmp = outstr;
    i = 1;
    while(*cmdstring){
        if(*cmdstring == '$'){
            if(!Q_strncmp(cmdstring, "$uid", 4)){
                Com_sprintf(tmp, sizeof(outstr) - (tmp - outstr), "%i", svse.cmdInvoker.currentCmdInvoker);
                tmp += strlen(tmp);
                cmdstring += 4;
            }else if(!Q_strncmp(cmdstring, "$clnum", 6)){
                Com_sprintf(tmp, sizeof(outstr) - (tmp - outstr), "%i", svse.cmdInvoker.clientnum);
                tmp += strlen(tmp);
                cmdstring += 6;
            }else if(!Q_strncmp(cmdstring, "$pow", 4)){
                Com_sprintf(tmp, sizeof(outstr) - (tmp - outstr), "%i", svse.cmdInvoker.currentCmdPower);
                tmp += strlen(tmp);
                cmdstring += 4;
            }else if(!Q_strncmp(cmdstring, "$arg", 4)){
                if(!*Cmd_Argv(i)){
                    Com_Printf("Not enought arguments to this command\n");
                    return;
                }
                if(strchr(Cmd_Argv(i), ';') || strchr(Cmd_Argv(i), '\n')){
                    return;
                }
                Com_sprintf(tmp, sizeof(outstr) - (tmp - outstr), "%s", Cmd_Argv(i));
                cmdstring += 4;
                tmp += strlen(tmp);
                i++;
            }
        }

        *tmp = *cmdstring;
        cmdstring++;
        tmp++;

    }

    *tmp = 0;
    Com_DPrintf("String to Execute: %s\n", outstr);
    Cbuf_AddText(EXEC_NOW, outstr);
}



/*
============
Cmd_AddTranslatedCommand_f
============
*/

static void Cmd_AddTranslatedCommand_f() {

    char *cmdname;
    char *string;
    int free;
    int i;

    if ( Cmd_Argc() != 3) {
        Com_Printf( "Usage: addCommand <commandname> <\"string to execute\"> String can contain: $uid $clnum $pow $arg\n" );
        return;
    }

    cmdname = Cmd_Argv(1);
    string = Cmd_Argv(2);

    for(i=0, free = -1; i < MAX_TRANSCMDS; i++){
        if(!Q_stricmp(cmdname, psvs.translatedCmd[i].cmdname)){
            Com_Printf("This command is already defined\n");
            return;
        }
        if(!*psvs.translatedCmd[i].cmdname){
            free = i;
        }

    }
    if(free == -1){
        Com_Printf("Exceeded limit of custom commands\n");
        return;
    }

    Q_strncpyz(psvs.translatedCmd[free].cmdname, cmdname, sizeof(psvs.translatedCmd[free].cmdname));
    Q_strncpyz(psvs.translatedCmd[free].cmdargument, string, sizeof(psvs.translatedCmd[free].cmdargument));

    Cmd_AddCommand (psvs.translatedCmd[free].cmdname, Cmd_ExecuteTranslatedCommand_f);
    Cmd_SetPower(psvs.translatedCmd[free].cmdname, 100);
    Com_Printf("Added custom command: %s -> %s\n", psvs.translatedCmd[free].cmdname, psvs.translatedCmd[free].cmdargument);

}

/*
====================
SV_StopRecording_f

stop recording a demo
====================
*/
void SV_StopRecord_f( void ) {

	clanduid_t cl;
	int i;

	if ( Cmd_Argc() != 2) {
		Com_Printf( "stoprecord <client>\n" );
		return;
	}

	if(!Q_stricmp(Cmd_Argv( 1 ), "all"))
	{
		for(i = 0, cl.cl = svs.clients; i < sv_maxclients->integer; i++, cl.cl++)
		{
			if(cl.cl->demorecording)
				SV_StopRecord(cl.cl);
		}
		return;
	}


	cl = SV_GetPlayerByHandle();
	if(!cl.cl){
		Com_Printf("Error: This player is not online and can not be recorded\n");
		return;
	}
	SV_StopRecord(cl.cl);
}


/*
====================
SV_Record_f

record <demoname>

Begins recording a demo from the current position
====================
*/
//static char demoName[MAX_QPATH];        // compiler bug workaround
void SV_Record_f( void ) {

	char* s;
	char name[MAX_QPATH];
	clanduid_t cl;
	int i;

	if ( Cmd_Argc() > 3 || Cmd_Argc() < 2) {
		Com_Printf( "record <client> <demoname>\n" );
		return;
	}

	if ( Cmd_Argc() == 3 ) {
		s = Cmd_Argv( 2 );
	} else {
		s = NULL;
	}

	if(!Q_stricmp(Cmd_Argv( 1 ), "all"))
	{
		for(i = 0, cl.cl = svs.clients; i < sv_maxclients->integer; i++, cl.cl++)
		{
			if(cl.cl->state == CS_ACTIVE && !cl.cl->demorecording){

				if(psvs.useuids){

					if(cl.cl->uid > 0)
						Com_sprintf(name, sizeof(name), "demo_%i_", cl.cl->uid);
					else
						Com_sprintf(name, sizeof(name), "demo_NA_%i_", i);
				}else{
					Com_sprintf(name, sizeof(name), "demo_%s_", cl.cl->pbguid);
				}
				SV_RecordClient(cl.cl, name);
			}
		}
		return;
	}

	cl = SV_GetPlayerByHandle();
	if(!cl.cl){
		Com_Printf("Error: This player is not online and can not be recorded\n");
		return;
	}

	if(s)
		SV_RecordClient(cl.cl, s);
	else{
		if(psvs.useuids){
			if(cl.cl->uid > 0)
				Com_sprintf(name, sizeof(name), "demo_%i_", cl.cl->uid);
			else
				Com_sprintf(name, sizeof(name), "demo_NA_%i_", svs.clients - cl.cl);
		}else{
			Com_sprintf(name, sizeof(name), "demo_%s_", cl.cl->pbguid);
		}
		SV_RecordClient(cl.cl, name);
	}
}



void SV_ShowRules_f(){

    unsigned int clnum;
    client_t* cl;
    int i;

    if(svse.cmdInvoker.currentCmdPower > 20){

	for(i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++)
	{
		if(cl->state == CS_ACTIVE){
	            cl->msgType = 1;
	            cl->currentAd = 0;
		}
	}

    }else{
        clnum = svse.cmdInvoker.clientnum;
        if(clnum < 64){
            cl = &svs.clients[clnum];
            cl->msgType = 1;
            cl->currentAd = 0;
        }
    }
}

void SV_AddRule_f(){

    if ( Cmd_Argc() != 2) {
        Com_Printf( "Usage: addRuleMsg <\"text here in quotes\">\n" );
        return;
    }

    G_AddRule( Cmd_Argv(1));
}

void SV_AddAdvert_f(){

    if ( Cmd_Argc() != 2) {
        Com_Printf( "Usage: addAdvertMsg <\"text here in quotes\">\n" );
        return;
    }
    G_AddAdvert( Cmd_Argv(1));
}

void SV_ClearAllMessages_f(){

    Com_Memset(&psvs.messages, 0, sizeof(psvs.messages));

}


/*
===========
SV_Serverinfo_f

Examine the serverinfo string
===========
*/
static void SV_Serverinfo_f( void ) {
	Com_Printf( "Server info settings:\n" );
	Info_Print( Cvar_InfoString( 0, CVAR_SERVERINFO ) );
}


/*
===========
SV_Systeminfo_f

Examine or change the serverinfo string
===========
*/
static void SV_Systeminfo_f( void ) {
	Com_Printf( "System info settings:\n" );
	Info_Print( Cvar_InfoString( 0, CVAR_SYSTEMINFO ) );
}

//===========================================================


/*
=================
SV_KillServer
=================
*/
static void SV_KillServer_f( void ) {
	SV_Shutdown("killserver");
	Com_Restart(  );
}


static void SV_Map_f( void ) {

	const char* map;
	const char* cmd;

	map = Cmd_Argv( 1 );
	if ( !map ) {
		return;
	}

	SV_Map(map);

	// set the cheat value
	// if the level was started with "map <levelname>", then
	// cheats will not be allowed.  If started with "devmap <levelname>"
	// then cheats will be allowed
	cmd = Cmd_Argv( 0 );
	if ( !Q_stricmp( cmd, "devmap" ) ) {

		Cvar_SetBool( sv_cheats, qtrue );
	} else {

		Cvar_SetBool( sv_cheats, qfalse );
	}
}


/*
================
SV_MapRotate_f

================
*/
static void SV_MapRotate_f( void ) {

	char map[MAX_QPATH];
	char gametype[MAX_QPATH];
	int len;
	char  *maplist;

	// DHM - Nerve :: Check for invalid gametype
	Com_Printf("map_rotate...\n");
	Com_Printf("\"sv_mapRotation\" is: \"%s\"\n\n", sv_mapRotation->string);
	Com_Printf("\"sv_mapRotationCurrent\" is: \"%s\"\n\n", sv_mapRotationCurrent->string);

	maplist = sv_mapRotationCurrent->string;
	Com_ParseReset();
	maplist = Com_ParseGetToken(maplist);

	if(maplist == NULL){
		Cvar_SetString(sv_mapRotationCurrent, sv_mapRotation->string);

		maplist = sv_mapRotationCurrent->string;
		Com_ParseReset();
		maplist = Com_ParseGetToken(maplist);
	}

	if(maplist == NULL){
		if(com_sv_running->boolean){
			Com_PrintError("\"sv_mapRotation\" is empty. Restarting current map\n");
			SV_MapRestart( qfalse );
		}else{
			Com_PrintError("\"sv_mapRotation\" is empty. Can not start the server\n");
		}
		return;
	}

	while(qtrue)
	{
		if(maplist == NULL)
			break;

		if(!Q_stricmpn(maplist, "gametype ", 9)){

			maplist = Com_ParseGetToken(maplist);
			if(maplist == NULL)
				break;

			len = Com_ParseTokenLength(maplist);
			if(len >= sizeof(gametype)){
				len = sizeof(gametype) -1;
				Com_PrintWarning("Oversize gametype name length at: %s\n", maplist);
			}
			Q_strncpyz(gametype, maplist, len+1);
			Cvar_SetString(g_gametype, gametype);
			maplist = Com_ParseGetToken(maplist); //Pop off the gametype argument

		}else if(!Q_stricmpn(maplist, "map ", 4)){

			maplist = Com_ParseGetToken(maplist);
			if(maplist == NULL)
				break;

			len = Com_ParseTokenLength(maplist);
			if(len >= sizeof(map)){
				len = sizeof(map) -1;
				Com_PrintWarning("Oversize map name length at: %s\n", maplist);
			}
			Q_strncpyz(map, maplist, len+1);

			if(!SV_Map(map)){ //Load the level
				Com_PrintError("Invalid mapname at %s\nRestarting current map\n", maplist);
				SV_MapRestart( qfalse );
			}
			maplist = Com_ParseGetToken(maplist); //Pop off the last map-name

			if(maplist == NULL)
				maplist = "";

			Cvar_SetString(sv_mapRotationCurrent, maplist); //Set the cvar with one map less
			return;
		}else{
			Com_PrintError("Broken maprotation at: %s\n", maplist);
			maplist = Com_ParseGetToken(maplist); //Pop off the last argument
		}
	}


	if(com_sv_running->boolean){
		Com_PrintError("\"sv_mapRotation\" is broken at: %s.\nRestarting current map\n");
		SV_MapRestart( qfalse );
	}else{
		Com_PrintError("\"sv_mapRotation\" is empty. Can not start the server\n");
	}
}

static void SV_MapRestart_f(void){

	SV_MapRestart(qfalse);

}

static void SV_FastRestart_f(void){

	SV_MapRestart(qtrue);

}


static void SV_SetPerk_f( void ){

    clanduid_t cl;
    unsigned int perkIndex;
    unsigned int clnum;

    cl = SV_GetPlayerByHandle();
    if(!cl.cl)
        return;

    if(Cmd_Argc() < 3){
        Com_DPrintf("Unknown Perk\n");
        return;
    }
    perkIndex = BG_GetPerkIndexForName(Cmd_Argv(2));
    if(perkIndex > 19){
        Com_DPrintf("Unknown Perk: %s\n", Cmd_Argv(2));
        return;
    }

    clnum = cl.cl - svs.clients;

    playerState_t *ps = SV_GameClientNum(clnum);

    ps->perks |= (1 << perkIndex);

    level.clients[clnum].sess.perkIndex |= (1 << perkIndex);

}



void SV_TestTimeOverrun( void ){

	svs.time = 0x6ffeffff;

}



void SV_GetCurrentServeTimer(){

	Com_Printf("Server Time is : %x\n", svs.time);
}


static void SV_SetGravity_f( void ){

    clanduid_t cl;
    int gravity;
    unsigned int clnum;

    cl = SV_GetPlayerByHandle();
    if(!cl.cl)
        return;

    if(Cmd_Argc() < 3){
        Com_Printf("Bad args\n");
        return;
    }

    gravity = atoi(Cmd_Argv(2));

    clnum = cl.cl - svs.clients;

    playerState_t *ps = SV_GameClientNum(clnum);

    ps->gravity = gravity;

    Com_Printf("Gravity: %i\n", ps->gravity);

}
/*
static void SV_SetStance_f( void ){

    clanduid_t cl;
    int gravity;
    unsigned int clnum;

    cl = SV_GetPlayerByHandle();
    if(!cl.cl)
        return;

    if(Cmd_Argc() < 3){
        Com_Printf("Bad args\n");
        return;
    }

    gravity = atoi(Cmd_Argv(2));

    clnum = cl.cl - svs.clients;

    playerState_t *ps = SV_GameClientNum(clnum);

    ps->stance = gravity;

    Com_Printf("Gravity: %i, Stance: %i\n", ps->gravity, ps->stance);

}
*/



static void SV_Find_f( void ){

    int i;

    for(i = 0; i < sv_maxclients->integer; i++){

        playerState_t *ps = SV_GameClientNum(i);

        if(svs.clients[i].state < CS_ACTIVE)
            continue;

        Com_Printf("Clientnum: %i \n", ps->clientNum);
    }
}

void Mac_Register_f(){
    if(Cmd_Argc()!=3){
	Com_Printf("Usage: %s <login - no spaces allowed> <password - no spaces allowed>\n",Cmd_Argv(0));
	return;
    }
    int uid = Mac_Register(Cmd_Argv(1),Cmd_Argv(2));
    if(uid == -1){
        Com_Printf("Error: user %s is already registered!",Cmd_Argv(1));
        return;
    }
    if(uid<0){
        Com_Printf("Error: failed to register the client.");
    }
    Com_Printf("UID for the registered user: %d.\n",uid);

}



void NET_Sockinfo_f(void);


void SV_AddOperatorCommands(){

	static qboolean	initialized;

	if ( initialized ) {
		return;
	}
	initialized = qtrue;

//	Cmd_AddCommand ("stance", SV_SetStance_f);

	Cmd_AddCommand ("systeminfo", SV_Systeminfo_f);
	Cmd_AddCommand ("serverinfo", SV_Serverinfo_f);
	Cmd_AddCommand ("killserver", SV_KillServer_f);
	Cmd_AddCommand ("setPerk", SV_SetPerk_f);
	Cmd_AddCommand ("map", SV_Map_f);
	Cmd_AddCommand ("map_restart", SV_MapRestart_f);
	Cmd_AddCommand ("fast_restart", SV_FastRestart_f);
	Cmd_AddCommand ("map_rotate", SV_MapRotate_f);
	Cmd_AddCommand ("addAdvertMsg", SV_AddAdvert_f);
	Cmd_AddCommand ("addRuleMsg", SV_AddRule_f);
	Cmd_AddCommand ("clearAllMsg", SV_ClearAllMessages_f);
	Cmd_AddCommand ("rules", SV_ShowRules_f);
	Cmd_AddCommand ("heartbeat", SV_Heartbeat_f);
	Cmd_AddCommand ("dumpbanlist", SV_DumpBanlist_f);
	Cmd_AddCommand ("kick", Cmd_KickPlayer_f);
	Cmd_AddCommand ("clientkick", Cmd_KickPlayer_f);
	Cmd_AddCommand ("onlykick", Cmd_KickPlayer_f);
	Cmd_AddCommand ("unban", Cmd_UnbanPlayer_f);
	Cmd_AddCommand ("unbanUser", Cmd_UnbanPlayer_f);
	Cmd_AddCommand ("permban", Cmd_BanPlayer_f);
	Cmd_AddCommand ("tempban", Cmd_TempBanPlayer_f);
	Cmd_AddCommand ("bpermban", Cmd_BanPlayer_f);
	Cmd_AddCommand ("btempban", Cmd_TempBanPlayer_f);
	Cmd_AddCommand ("banUser", Cmd_BanPlayer_f);
	Cmd_AddCommand ("banClient", Cmd_BanPlayer_f);
	Cmd_AddCommand ("ministatus", SV_MiniStatus_f);
	Cmd_AddCommand ("writenvcfg", NV_WriteConfig);
	Cmd_AddCommand ("setAdmin", Cmd_RemoteSetAdmin_f);
	Cmd_AddCommand ("unsetAdmin", Cmd_RemoteUnsetAdmin_f);
	Cmd_AddCommand ("setCmdMinPower", Cmd_RemoteSetPermission_f);
	Cmd_AddCommand ("adminlist", Cmd_RemoteListAdmin_f);
	Cmd_AddCommand ("cmdpowerlist", Cmd_ListPower_f);
	Cmd_AddCommand ("say", SV_ConSayChat_f);
	Cmd_AddCommand ("consay", SV_ConSayConsole_f);
	Cmd_AddCommand ("screensay", SV_ConSayScreen_f);
	Cmd_AddCommand ("tell", SV_ConTellChat_f);
	Cmd_AddCommand ("contell", SV_ConTellConsole_f);
	Cmd_AddCommand ("screentell", SV_ConTellScreen_f);
	Cmd_AddCommand ("dumpuser", SV_DumpUser_f);
	Cmd_AddCommand ("status", SV_Status_f);
	Cmd_AddCommand ("addCommand", Cmd_AddTranslatedCommand_f);
//	Cmd_AddCommand ("UidSvConStatus", Cmd_DisplayUIDSVConStatus_f);
//	Cmd_AddCommand ("fast_restarttest", (void*)0x816c67e);
	Cmd_AddCommand ("stringUsage", SV_StringUsage_f);
	Cmd_AddCommand ("scriptUsage", SV_ScriptUsage_f);
//	Cmd_AddCommand ("macRegister",Mac_Register_f);

	Cmd_AddCommand ("stoprecord", SV_StopRecord_f);
	Cmd_AddCommand ("record", SV_Record_f);


	if(com_developer && com_developer->integer){

		Cmd_AddCommand ("devmap", SV_Map_f);

	}

}


