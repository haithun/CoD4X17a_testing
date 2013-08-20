#include "q_shared.h"
#include "qcommon_io.h"
#include "cvar.h"
#include "sys_net.h"
#include "server.h"
#include "g_shared.h"

#include <string.h>
#include <stdlib.h>

#ifndef MAX_PACKETLEN
#define MAX_PACKETLEN 1400
#endif

/*********************************************************************************************/
//		Mandatory functions for PunkBuster operation
/*********************************************************************************************/

__cdecl int PbSvSendToAddrPort(char* netdest, unsigned short port, int msgsize, char* message){

//    char *sourcemsg;
//    char msg[256];
    netadr_t netadr;

/*
    __asm("leal -0x836(,%%ebp,1), %%eax\n\t" :"=a"(sourcemsg));

    if(!Q_strncmp(sourcemsg, "PunkBuster Server:", 18)){
        Q_strncpyz(msg, sourcemsg, sizeof(msg));
        if(strstr(msg,"NoGUID*"))		//Prevent telling about Players without GUIDs to streaming-servers
            return 0;

        if(SV_ClientAuthMode() < 1)	//Prevent streaming of any violations if players can have bad guids
            return 0;
    }
*/

    NET_StringToAdr(va("%s:%i", netdest, port), &netadr, NA_UNSPEC);

    netadr.sock = 0;

    netadr_t *sockadr;

    if((sockadr = NET_GetDefaultCommunicationSocket()) != NULL)
        netadr.sock = sockadr->sock;

    NET_SendPacket(NS_SERVER, msgsize, message, &netadr);

    return 0;
}

__cdecl int PbSvSendToClient(int msgsize, char* message, int clientnum){
    client_t *cl;
    cl = &svs.clients[clientnum];

    if(cl->state >= CS_CONNECTED){

        byte string[MAX_PACKETLEN];
        int i;

        // set the OutOfBand header
        string[0] = 0xff;
        string[1] = 0xff;
        string[2] = 0xff;
        string[3] = 0xff;

        if(msgsize + 4 > MAX_PACKETLEN){
            Com_PrintWarning("Buffer Overflow in NET_OutOfBandData %i bytes\n", msgsize);
            return 0;
        }

        for ( i = 0; i < msgsize ; i++ ) {
            string[i+4] = message[i];
        }

        NET_SendPacket( NS_SERVER, i+4, string, &cl->netchan.remoteAddress );

    }
    return 0;
}


__cdecl char* PbSvGameQuery(int para_01, char* string){

    int maxclients;
    client_t *cl;
    gclient_t *gclient;
    string[255] = 0;
    int		var_01;
    if(!string) return NULL;
    switch(para_01){
	case 0x65:
            maxclients = sv_maxclients->integer;
	    if(!maxclients) return 0;
	    *string = 0x30;
	    Com_sprintf(string, 255, "%i", maxclients);
	    return 0;

	case 0x66:
	    maxclients = sv_maxclients->integer;
	    var_01 = atoi(string);
	    Com_Memset(string, 0, 0x68);
	    if(var_01 < 0 || var_01 > maxclients) return "PB_Error: Query Failed";
	    cl = &svs.clients[var_01];

	    if(cl->state < CS_ACTIVE || cl->noPb == qtrue) return "PB_Error: Query Failed";
	    Q_strncpyz(string, cl->name, 254);
	    Q_strncpyz(&string[33], cl->pbguid, 221);
	    Q_strncpyz(&string[66], NET_AdrToString(&cl->netchan.remoteAddress), 188);
	    return NULL;

	case 0x67:
	    Q_strncpyz(string, Cvar_GetVariantString(string),255);
	    return NULL;

	case 0x72:

	    maxclients = sv_maxclients->integer;
	    *string = 0;
	    var_01 = atoi(string);

	    if(var_01 < 0 || var_01 > maxclients) return "PB_Error: Query Failed";;

	    cl = &svs.clients[var_01];
	    gclient = &level.clients[para_01];

	    if(cl->state < CS_ACTIVE || cl->noPb) return "PB_Error: Query Failed";;

	    Com_sprintf(string,255,"ping=%d score=%d", cl->ping, gclient->pers.scoreboard.score);
	    return NULL;

	default:
	    return NULL;
    }
}

