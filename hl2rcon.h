#include "q_shared.h"
#include "sys_net.h"
#include "msg.h"

/*
========================================================================

Source Rcon facility

========================================================================
*/

#define MAX_RCONUSERS 8
#define MAX_RCONLOGINS 64

typedef struct{
	netadr_t remote;
	int socketfd;
	int lastpacketid;
	qboolean streamlog;
	qboolean streamchat;
	qboolean streamgamelog;
	qboolean streamevents;
	int rconPower; //unused for now
	char rconUsername[32];
}rconUser_t;

typedef struct{
	char username[32];
	char salt[129];
	char sha256[65];
	int power;
}rconLogin_t;

typedef struct{
	rconLogin_t rconUsers[MAX_RCONLOGINS];
	rconUser_t activeRconUsers[MAX_RCONUSERS];
	//For redirect
	int redirectUser;
}sourceRcon_t;

typedef enum{
    SERVERDATA_RESPONSE_VALUE = 0,
    SERVERDATA_EXECCOMMAND = 2,
    SERVERDATA_AUTH_RESPONSE = 2,
    SERVERDATA_AUTH = 3,

    SERVERDATA_CHANGEPASSWORD = 63,
    SERVERDATA_TURNONSTREAM = 64,
    SERVERDATA_GAMELOG = 65,
    SERVERDATA_CONLOG = 66,
    SERVERDATA_CHAT = 67,
    SERVERDATA_GETSTATUS = 68,
    SERVERDATA_STATUSRESPONSE = 69,
    SERVERDATA_SAY = 70,
    SERVERDATA_EVENT = 71
}sourceRconCommands_t;

typedef enum{
    RCONEVENT_PLAYERENTERGAME = 0,
    RCONEVENT_PLAYERLEAVE = 1,
    RCONEVENT_LEVELSTART = 2,
    RCONEVENT_PLAYERENTERTEAM = 3
}sourceRconEvents_t;


void HL2Rcon_SetSourceRconAdmin_f( void );
void HL2Rcon_UnsetSourceRconAdmin_f( void );
void HL2Rcon_ListSourceRconAdmins_f( void );
void HL2Rcon_ChangeSourceRconAdminPassword( const char* password );
void HL2Rcon_SourceRconStreaming_enable( int type );
qboolean HL2Rcon_AddSourceRconAdminToList(const char* username, const char* password, const char* salt, int power);
void HL2Rcon_ClearSourceRconAdminList( void );
void HL2Rcon_SourceRconDisconnect(netadr_t *from, int socketfd, int connectionId);
tcpclientstate_t HL2Rcon_SourceRconAuth(netadr_t *from, msg_t *msg, int *socketfd, int *connectionId);
void HL2Rcon_SourceRconSendConsole( const char* data, int msglen);
void HL2Rcon_SourceRconSendChat( const char* data, int clientnum);

void HL2Rcon_SourceRconSendDataToEachClient( const byte* data, int msglen, int type);
void HL2Rcon_SourceRconFlushRedirect(char* outputbuf, qboolean lastcommand);
qboolean HL2Rcon_SourceRconEvent(netadr_t *from, msg_t *msg, int *socketfd, int connectionId);
void HL2Rcon_AddSourceAdminCommands();
void HL2Rcon_SourceRconSendGameLog( const char* data, int msglen);
void HL2Rcon_SourceRconSendChatToEachClient( const char *text, rconUser_t *self, int cid, qboolean onlyme);
void HL2Rcon_EventClientEnterTeam(int cid, int team);
void HL2Rcon_EventClientEnterWorld(int cid);
void HL2Rcon_EventClientLeave(int cid);
void HL2Rcon_EventLevelStart();