#include "hl2rcon.h"
#include "q_shared.h"
#include "qcommon.h"
#include "qcommon_io.h"
#include "cmd.h"
#include "nvconfig.h"
#include "msg.h"
#include "sys_net.h"
#include "server.h"
#include "net_game_conf.h"
#include "sha256.h"
#include "punkbuster.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
/*
========================================================================

Source Rcon facility

========================================================================
*/

#ifndef MAX_MSGLEN
#define MAX_MSGLEN 0x20000
#endif


sourceRcon_t sourceRcon;


void HL2Rcon_SetSourceRconAdmin_f( void ){

	const char* username;
	const char* password;
	const char* sha256;
	byte salt[129];
	int power, i;
	rconLogin_t* user;
	rconLogin_t* free = NULL;


	if(Cmd_Argc() != 4){
		Com_Printf("Usage: rconaddadmin <username, password, power>\n");
		return;
	}

	username = Cmd_Argv(1);
	password = Cmd_Argv(2);
	power = atoi(Cmd_Argv(3));


	if(!username || !*username || !password || strlen(password) < 6 || power < 1 || power > 100){
		Com_Printf("Usage: rconaddadmin <username, password (at least 6 characters), power (and integer between 1 and 100)>\n");
		return;
	}

	NV_ProcessBegin();

	for(i = 0, user = sourceRcon.rconUsers; i < MAX_RCONLOGINS; i++, user++){

		if(!Q_stricmp(user->username, username)){
			Com_Printf("A rconadmin with this username is already registered\n");
			return;
		}

		if(!free && !*user->username )
			free = user;
	}
	if(!free){
		Com_Printf("Too many registered rconadmins. Limit is: %d\n", MAX_RCONLOGINS);
		return;
	}

	Com_RandomBytes(salt, sizeof(salt));

	for(i = 0; i < sizeof(salt) -1; i++){
		if(salt[i] > 126){
		    salt[i] -= 125;
		}
		if(salt[i] < 21){
		    salt[i] += 21;
		}
		if(salt[i] == ';')
			salt[i]++;

		if(salt[i] == '\\')
			salt[i]++;

		if(salt[i] == '%')
			salt[i]++;

		if(salt[i] == '"')
			salt[i]++;
	}

	salt[sizeof(salt) -1] = 0;

	sha256 = Com_SHA256(va("%s.%s", password, salt));

	Q_strncpyz(free->username, username, sizeof(free->username));
	Q_strncpyz(free->sha256, sha256, sizeof(free->sha256));
	Q_strncpyz(free->salt, (char*)salt, sizeof(free->salt));
	free->power = power;

	NV_ProcessEnd();
}


void HL2Rcon_UnsetSourceRconAdmin_f( void ){

	const char* username;
	int i;
	rconLogin_t* user;

	if(Cmd_Argc() != 2){
		Com_Printf("Usage: rcondeladmin < username >\n");
		return;
	}

	username = Cmd_Argv(1);

	NV_ProcessBegin();

	for(i = 0, user = sourceRcon.rconUsers; i < MAX_RCONLOGINS; i++, user++){

		if(!Q_stricmp(user->username, username)){
			Com_Printf("Removed %s from the list of rconadmins\n", user->username);
			Com_Memset(user, 0, sizeof(rconLogin_t));
			NV_ProcessEnd();
			return;
		}
	}
	Com_Printf("No such admin: %s\n", username);
}


void HL2Rcon_ListSourceRconAdmins_f( void ){

	int i;
	rconLogin_t* user;

	Com_Printf("------- SourceRconAdmins: -------\n");
	for(i = 0, user = sourceRcon.rconUsers; i < MAX_RCONLOGINS; i++, user++){
		if(*user->username)
			Com_Printf("  %2d:   Name: %s, Power: %d\n", i+1, user->username, user->power);
	}
	Com_Printf("---------------------------------\n");
}


void HL2Rcon_ChangeSourceRconAdminPassword( const char* password ){

	const char* sha256;
	char salt[129];
	rconLogin_t* user;
	int i;

	if(sourceRcon.redirectUser < 1 || sourceRcon.redirectUser > MAX_RCONUSERS){
		Com_Printf("This command can only be used from SourceRcon\n");
		return;
	}

	if(!password || strlen(password) < 6){
		Com_Printf("Error: password too short. (at least 6 characters)\n");
		return;
	}

	NV_ProcessBegin();

	user = &sourceRcon.rconUsers[sourceRcon.redirectUser -1];


	Com_RandomBytes((byte*)salt, sizeof(salt));
	salt[sizeof(salt) -1] = 0;

	for(i = 0; i < sizeof(salt) -1; i++){
		if(salt[i] > 126){
		    salt[i] -= 125;
		}
		if(salt[i] < 21){
		    salt[i] += 21;
		}
		if(salt[i] == ';')
			salt[i]++;

		if(salt[i] == '\\')
			salt[i]++;

		if(salt[i] == '%')
			salt[i]++;

		if(salt[i] == '"')
			salt[i]++;
	}

	sha256 = Com_SHA256(va("%s.%s", password, salt));

	Q_strncpyz(user->sha256, sha256, sizeof(user->sha256));
	Q_strncpyz(user->salt, salt, sizeof(user->salt));

	NV_ProcessEnd();

	Com_Printf("Password changed to: %s\n", password);
}



void HL2Rcon_SourceRconStreaming_enable( int type ){

	rconUser_t* user;
	char* c;
	char* cg;
	char* ch;
	char* ev;

	if(sourceRcon.redirectUser < 1 || sourceRcon.redirectUser > MAX_RCONUSERS){
		Com_Printf("This command can only be used from SourceRcon\n");
		return;
	}

	user = &sourceRcon.activeRconUsers[sourceRcon.redirectUser -1];

	user->streamlog = type & 1;
	user->streamgamelog = type & 2;
	user->streamchat = type & 4;
	user->streamevents = type & 8;

	if(user->streamlog)
		c = "logfile";
	else
		c = "";

	if(user->streamgamelog)
		cg = "gamelog";
	else
		cg = "";

	if(user->streamchat)
		ch = "chat";
	else
		ch = "";

	if(user->streamevents)
		ev = "events";
	else
		ev = "";

	Com_Printf("Streaming turned on for: %s %s %s %s\n", c, cg, ch, ev);
}

void HL2Rcon_ClearSourceRconAdminList( )
{
    Com_Memset(sourceRcon.rconUsers, 0, sizeof(sourceRcon.rconUsers));
}

qboolean HL2Rcon_AddSourceRconAdminToList(const char* username, const char* password, const char* salt, int power){

	rconLogin_t* user;
	rconLogin_t* free = NULL;
	int i;

	if(!username || !*username || !password || strlen(password) < 6 || power < 1 || power > 100 || !salt || strlen(salt) != 128)
		return qfalse;

	for(i = 0, user = sourceRcon.rconUsers; i < MAX_RCONLOGINS; i++, user++){

		if(!Q_stricmp(user->username, username)){
			return qfalse;
		}

		if(!free && !*user->username )
			free = user;
	}
	if(!free)
		return qfalse;

	Q_strncpyz(free->username, username, sizeof(free->username));
	Q_strncpyz(free->sha256, password, sizeof(free->sha256));
	Q_strncpyz(free->salt, salt, sizeof(free->salt));
	free->power = power;
	return qtrue;
}



void HL2Rcon_SourceRconDisconnect(netadr_t *from, int socketfd, int connectionId){

	if(connectionId < 0 || connectionId >=  MAX_RCONUSERS){
		Com_Error(ERR_FATAL, "HL2Rcon_SourceRconDisconnect: bad connectionId: %i", connectionId);
		return;
	}
	sourceRcon.activeRconUsers[connectionId].remote.type = NA_BAD;
	sourceRcon.activeRconUsers[connectionId].streamlog = 0;
	sourceRcon.activeRconUsers[connectionId].streamchat = 0;
	sourceRcon.activeRconUsers[connectionId].streamgamelog = 0;
	sourceRcon.activeRconUsers[connectionId].streamevents = 0;

}


tcpclientstate_t HL2Rcon_SourceRconAuth(netadr_t *from, msg_t *msg, int *socketfd, int *connectionId){

	int packetlen;
	int packettype;
	int packetid;
	char* loginstring;
	char* username;
	char* password;
	const char* sha256;
	char hstring[256];
	byte msgbuf[32];
	msg_t sendmsg;
	rconUser_t* user;
	rconLogin_t* login;
	int i;

	if(SV_PlayerBannedByip(from)){
		return TCP_AUTHBAD;
	}
	MSG_BeginReading(msg);
	packetlen = MSG_ReadLong(msg);

	if(packetlen != msg->cursize - 4)//Not a source rcon packet
		return TCP_AUTHNOTME;

	packetid = MSG_ReadLong(msg);

	packettype = MSG_ReadLong(msg);

	if(packettype != SERVERDATA_AUTH)//Not a source rcon auth-packet
		return TCP_AUTHNOTME;

	MSG_Init(&sendmsg, msgbuf, sizeof(msgbuf));
	MSG_WriteLong(&sendmsg, 10);
	MSG_WriteLong(&sendmsg, 0);
	MSG_WriteLong(&sendmsg, SERVERDATA_RESPONSE_VALUE);
	MSG_WriteShort(&sendmsg, 0);
	NET_SendData(socketfd, sendmsg.data, sendmsg.cursize);

	MSG_Init(&sendmsg, msgbuf, sizeof(msgbuf));
	MSG_WriteLong(&sendmsg, 10);

	loginstring = MSG_ReadStringLine(msg);

	Cmd_TokenizeString(loginstring);

	if(Cmd_Argc() != 2){
		goto badrcon;
	}
	username = Cmd_Argv(0);
	password = Cmd_Argv(1);

	if(strlen(password) < 6){
		goto badrcon;
	}

	for(i = 0, login = sourceRcon.rconUsers; i < MAX_RCONLOGINS; i++, login++){
		if(!Q_stricmp(login->username, username))
			break;
	}

	if(i == MAX_RCONLOGINS){
		goto badrcon;
	}
	Com_sprintf(hstring, sizeof(hstring), "%s.%s", password, login->salt);

	sha256 = Com_SHA256(hstring);

	if(Q_strncmp(login->sha256, sha256, 128))
		goto badrcon;

	Com_Printf("Rcon login from: %s Name: %s\n", NET_AdrToString (from), login->username);

	Cmd_EndTokenizeString();

	for(i = 0, user = sourceRcon.activeRconUsers; i < MAX_RCONUSERS; i++, user++){
		if(user->remote.type == NA_BAD)
			break;
	}

	if(i == MAX_RCONUSERS){
		return TCP_AUTHBAD; //Close connection
	}


	user->remote = *from;
	user->rconPower = login->power;
	Q_strncpyz(user->rconUsername, login->username, sizeof(user->rconUsername));
	user->socketfd = *socketfd;
	user->streamchat = 0;
	user->streamlog = 0;
	user->lastpacketid = packetid;
	*connectionId = i;

	MSG_WriteLong(&sendmsg, user->lastpacketid);
	MSG_WriteLong(&sendmsg, SERVERDATA_AUTH_RESPONSE);
	MSG_WriteShort(&sendmsg, 0);
	NET_SendData(socketfd, sendmsg.data, sendmsg.cursize);

	return TCP_AUTHSUCCESSFULL;


badrcon:
	Cmd_EndTokenizeString();
	Com_Printf ("Bad rcon from %s (TCP)\n", NET_AdrToString (from) );
	//Don't allow another attempt for 20 seconds
	SV_PlayerAddBanByip(*from, "Bad rcon", 0, 0, Com_GetRealtime() + 20);

	MSG_Init(&sendmsg, msgbuf, sizeof(msgbuf));
	MSG_WriteLong(&sendmsg, 10);
	MSG_WriteLong(&sendmsg, -1);
	MSG_WriteLong(&sendmsg, SERVERDATA_AUTH_RESPONSE);
	MSG_WriteShort(&sendmsg, 0);
	NET_SendData(socketfd, sendmsg.data, sendmsg.cursize);
	return TCP_AUTHBAD;

}



void HL2Rcon_SourceRconSendConsole( const char* data, int msglen)
{
	HL2Rcon_SourceRconSendDataToEachClient( (const byte*)data, msglen, SERVERDATA_CONLOG);
}

void HL2Rcon_SourceRconSendGameLog( const char* data, int msglen)
{
	HL2Rcon_SourceRconSendDataToEachClient( (const byte*)data, msglen, SERVERDATA_GAMELOG);
}



void HL2Rcon_SourceRconSendChat( const char* data, int clientnum)
{
    HL2Rcon_SourceRconSendChatToEachClient( data, NULL, clientnum, qfalse);
}

#define HL2RCON_SOURCEOUTPUTBUF_LENGTH 4096

void HL2Rcon_SourceRconSendDataToEachClient( const byte* data, int msglen, int type){

	rconUser_t* user;
	int i;
	msg_t msg;
	int32_t *updatelen;
	byte sourcemsgbuf[MAX_MSGLEN];
	qboolean msgbuild = qfalse;

	for(i = 0, user = sourceRcon.activeRconUsers; i < MAX_RCONUSERS; i++, user++ ){

		if(!user->streamgamelog && type == SERVERDATA_GAMELOG)
			continue;

		if(!user->streamlog && type == SERVERDATA_CONLOG)
			continue;

		if(!user->streamevents && type == SERVERDATA_EVENT)
			continue;

		
		if(!msgbuild){
			MSG_Init(&msg, sourcemsgbuf, sizeof(sourcemsgbuf));
			MSG_WriteLong(&msg, 0); //writing 0 for now
			MSG_WriteLong(&msg, 0);
			MSG_WriteLong(&msg, type);

			if(type == SERVERDATA_EVENT)
				MSG_WriteData(&msg, data, msglen);
			else
				MSG_WriteString(&msg, (char*)data);

			MSG_WriteByte(&msg, 0);

			//Adjust the length
			updatelen = (int32_t*)msg.data;
			*updatelen = msg.cursize - 4;
			msgbuild = qtrue;
		}
		NET_SendData(&user->socketfd, msg.data, msg.cursize);
	}
}




void HL2Rcon_SourceRconSendChatToEachClient( const char *text, rconUser_t *self, int cid, qboolean onlyme){

	rconUser_t* user;
	int i;
	msg_t msg;
	int32_t *updatelen;
	byte sourcemsgbuf[MAX_MSGLEN];



	for(i = 0, user = sourceRcon.activeRconUsers; i < MAX_RCONUSERS; i++, user++ ){

		if(!user->streamchat)
			continue;

		Com_Printf("Debug: %s\n", text);

		MSG_Init(&msg, sourcemsgbuf, sizeof(sourcemsgbuf));
		MSG_WriteLong(&msg, 0); //writing 0 for now
		MSG_WriteLong(&msg, 0);
		MSG_WriteLong(&msg, SERVERDATA_CHAT);

		if(self){
			if(self == user)
			{

				MSG_WriteByte(&msg, -2);
			}else{

				MSG_WriteByte(&msg, -1);
				if(onlyme)
				{
				    continue;
				}
			}
			MSG_WriteString(&msg, user->rconUsername);

		}else{

			MSG_WriteByte(&msg, cid);
		}


		MSG_WriteString(&msg, text);
		MSG_WriteByte(&msg, 0);

		//Adjust the length
		updatelen = (int32_t*)msg.data;
		*updatelen = msg.cursize - 4;

		NET_SendData(&user->socketfd, msg.data, msg.cursize);
	}
}


void HL2Rcon_SourceRconFlushRedirect(char* outputbuf, qboolean lastcommand){

	rconUser_t* user;

	if(sourceRcon.redirectUser < 1 || sourceRcon.redirectUser > MAX_RCONUSERS)
		return;

	user = &sourceRcon.activeRconUsers[sourceRcon.redirectUser -1];

	msg_t msg;
	int32_t *updatelen;
	byte sourcemsgbuf[HL2RCON_SOURCEOUTPUTBUF_LENGTH+16];

	MSG_Init(&msg, sourcemsgbuf, sizeof(sourcemsgbuf));
	MSG_WriteLong(&msg, 0); //writing 0 for now
	MSG_WriteLong(&msg, user->lastpacketid);
	MSG_WriteLong(&msg, SERVERDATA_RESPONSE_VALUE);
	MSG_WriteString(&msg, outputbuf);

	MSG_WriteByte(&msg, 0);

	//Adjust the length
	updatelen = (int32_t*)msg.data;
	*updatelen = msg.cursize - 4;

	NET_SendData(&user->socketfd, msg.data, msg.cursize);
}


void HL2Rcon_SayToPlayers(int clientnum, int team, const char* chatline)
{

	char		line[512];
	rconUser_t*	user;

	if(sourceRcon.redirectUser < 1 || sourceRcon.redirectUser > MAX_RCONUSERS){
		Com_Printf("This command can only be used from SourceRcon\n");
		return;
	}

	user = &sourceRcon.activeRconUsers[sourceRcon.redirectUser -1];

	if(clientnum != -1)
	{
		Com_sprintf(line, sizeof(line), "^5%s^7(Rcon): %s\n", user->rconUsername, chatline);
		HL2Rcon_SourceRconSendChatToEachClient( chatline, user, 0, qtrue);
	}else{
		Com_sprintf(line, sizeof(line), "^2%s^7(Rcon): %s\n", user->rconUsername, chatline);
		HL2Rcon_SourceRconSendChatToEachClient( chatline, user, 0, qfalse);
	}
        SV_SayToPlayers(clientnum, team, line);
}


qboolean HL2Rcon_SourceRconEvent(netadr_t *from, msg_t *msg, int *socketfd, int connectionId){

	int packetlen;
	int packettype;
	int type;
	int8_t team;
	int8_t clientnum;
	int32_t *updatelen;
	char* command;
	char* password;
	char* chatline;
	char sv_outputbuf[HL2RCON_SOURCEOUTPUTBUF_LENGTH];
	msg_t msg2;
	byte data[20000];

	MSG_BeginReading(msg);
	packetlen = MSG_ReadLong(msg);

	if(packetlen != msg->cursize - 4)//Not a source rcon packet
		return qtrue;

	if(connectionId < 0 || connectionId >= MAX_RCONUSERS)
		return qtrue;

	rconUser_t* user;
	user = &sourceRcon.activeRconUsers[connectionId];

	user->lastpacketid = MSG_ReadLong(msg);

	packettype = MSG_ReadLong(msg);
	
	switch(packettype)
	{
		case SERVERDATA_GETSTATUS:
		//status request


		    MSG_Init(&msg2, data, sizeof(data));
		    MSG_WriteLong(&msg2, 0); //writing 0 for now
		    MSG_WriteLong(&msg2, user->lastpacketid); // ID
		    MSG_WriteLong(&msg2, SERVERDATA_STATUSRESPONSE); // Type: status response
		    SV_WriteRconStatus(&msg2);
		    MSG_WriteByte(&msg2, 0);

		    //Adjust the length
		    updatelen = (int32_t*)msg2.data;
		    *updatelen = msg2.cursize - 4;
		    NET_SendData(socketfd, msg2.data, msg2.cursize);
		    return qfalse;

		case SERVERDATA_EXECCOMMAND:

		    command = MSG_ReadStringLine(msg);
		    Com_Printf("Rcon from: %s command: %s\n", NET_AdrToString(from), command);
		    sourceRcon.redirectUser = connectionId+1;
		    Com_BeginRedirect (sv_outputbuf, HL2RCON_SOURCEOUTPUTBUF_LENGTH, HL2Rcon_SourceRconFlushRedirect);
		    Cmd_ExecuteSingleCommand(0,0, command);

		    if(!Q_stricmpn(command, "pb_sv_", 6)) PbServerForceProcess();

		    Com_EndRedirect ();
		    sourceRcon.redirectUser = 0;
		    return qfalse;

		case SERVERDATA_CHANGEPASSWORD:

		    password = MSG_ReadString(msg);
		    sourceRcon.redirectUser = connectionId+1;
		    Com_BeginRedirect (sv_outputbuf, HL2RCON_SOURCEOUTPUTBUF_LENGTH, HL2Rcon_SourceRconFlushRedirect);
		    HL2Rcon_ChangeSourceRconAdminPassword( password );
		    Com_EndRedirect ();
		    sourceRcon.redirectUser = 0;
		    return qfalse;


		case SERVERDATA_TURNONSTREAM:

		    type = MSG_ReadByte(msg);
		    sourceRcon.redirectUser = connectionId+1;
		    Com_BeginRedirect (sv_outputbuf, HL2RCON_SOURCEOUTPUTBUF_LENGTH, HL2Rcon_SourceRconFlushRedirect);
		    HL2Rcon_SourceRconStreaming_enable( type );
		    Com_EndRedirect ();
		    sourceRcon.redirectUser = 0;
		    return qfalse;

		case SERVERDATA_SAY:
		    clientnum = MSG_ReadByte(msg); // -1 if Team or for all is used
		    team = MSG_ReadByte(msg); // teamnumber or -1 if it is for all team or clientnum is set
		    chatline = MSG_ReadString(msg);
		    sourceRcon.redirectUser = connectionId+1;
		    HL2Rcon_SayToPlayers(clientnum, team, chatline);
		    sourceRcon.redirectUser = 0;
		    return qfalse;

		default:
		//Not a source rcon packet
		Com_Printf("Not a valid source rcon packet from: %s received. Closing connection\n", NET_AdrToString(from));
		return qtrue;
	}

}


void HL2Rcon_AddSourceAdminCommands(){

	static qboolean	initialized;

	if ( initialized ) {
		return;
	}
	initialized = qtrue;

	Cmd_AddCommand ("rcondeladmin", HL2Rcon_UnsetSourceRconAdmin_f);
	Cmd_AddCommand ("rconaddadmin", HL2Rcon_SetSourceRconAdmin_f);
	Cmd_AddCommand ("rconlistadmins", HL2Rcon_ListSourceRconAdmins_f);
}


void HL2Rcon_EventClientEnterWorld(int cid){

    byte data[2];

    data[0] = RCONEVENT_PLAYERENTERGAME;
    data[1] = cid;

    HL2Rcon_SourceRconSendDataToEachClient( data, 2, SERVERDATA_EVENT);

}

void HL2Rcon_EventClientLeave(int cid){

    byte data[2];

    data[0] = RCONEVENT_PLAYERLEAVE;
    data[1] = cid;

    HL2Rcon_SourceRconSendDataToEachClient( data, 2, SERVERDATA_EVENT);

}

void HL2Rcon_EventLevelStart()
{

    byte data[1];

    data[0] = RCONEVENT_LEVELSTART;

    HL2Rcon_SourceRconSendDataToEachClient( data, 1, SERVERDATA_EVENT);

}

void HL2Rcon_EventClientEnterTeam(int cid, int team){

    byte data[2];

    data[0] = RCONEVENT_PLAYERENTERTEAM;
    data[1] = cid;
    data[2] = team;

    HL2Rcon_SourceRconSendDataToEachClient( data, 3, SERVERDATA_EVENT);

}
