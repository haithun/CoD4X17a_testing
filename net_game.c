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
#include "q_shared.h"
#include "qcommon.h"
#include "qcommon_io.h"
#include "msg.h"
#include "sys_net.h"
#include "server.h"
#include "net_game.h"

void NET_UDPPacketEvent(netadr_t* from, void* data, int len)
{

        msg_t msg;
        msg.data = data;
        msg.cursize = len;
        msg.maxsize = len;
        msg.readcount = 0;
        msg.bit = 0;
        msg.readonly = qtrue;
        msg.overflowed = qfalse;

        SV_PacketEvent(from, &msg);
}


unsigned int NET_TimeGetTime()
{
        return (unsigned int)com_frameTime;
}


/*
=================
NET_ReadTcpPackets
=================
*/

#define MAX_TCPEVENTS 4

typedef struct
{
    int serviceId;
    qboolean (*tcpauthevent)(netadr_t* from, msg_t* msg, int socketfd, int *connectionId);
    qboolean (*tcpevent)(netadr_t* from, msg_t* msg, int socketfd, int connectionId);

}tcpevent_t;

tcpevent_t tcpevents[MAX_TCPEVENTS];


void NET_TCPConnectionClosed(netadr_t* adr, int sock, int connectionId, int serviceId)
{
/*
	if(serviceId == 0x782a3){
		HL2Rcon_SourceRconDisconnect(from, socketfd, connectionId);
		return;
	}
	Com_PrintError("NET_TCPConnectionClosed: Bad serviceId: %d\n", serviceId);
*/
}

tcpclientstate_t NET_TCPAuthPacketEvent(netadr_t* from, byte* bufData, int len, int socketfd, int* connectionId, int *serviceId)
{

        int i;
        msg_t msg;
        tcpclientstate_t ret;

        msg.data = bufData;
        msg.cursize = len;
        msg.maxsize = len;
        msg.readcount = 0;
        msg.bit = 0;
        msg.readonly = qtrue;
        msg.overflowed = qfalse;

        Com_DPrintf("Packet event from: %s\n", NET_AdrToString(from));

        for(i = 0; i < MAX_TCPEVENTS; i++)
        {
            if(tcpevents[i].tcpauthevent == NULL)
                return qtrue; //Close connection

            ret = tcpevents[i].tcpauthevent(from, &msg, socketfd, connectionId);
            if(ret != TCP_AUTHNOTME)
            {
                *serviceId = tcpevents[i].serviceId;
                return ret;
            }

        }
        Com_DPrintf("^5Bad TCP-Packet from: %s\n", NET_AdrToString(from));
        return TCP_AUTHBAD;
}

qboolean NET_TCPPacketEvent(netadr_t* from, byte* bufData, int len, int socketfd, int connectionId, int serviceId)
{
        int i;
        msg_t msg;

        for(i = 0; i < MAX_TCPEVENTS; i++)
        {
            if(tcpevents[i].tcpevent == NULL)
                return qtrue; //Close connection

            if(tcpevents[i].serviceId == serviceId)
            {

                msg.data = bufData;
                msg.cursize = len;
                msg.maxsize = len;
                msg.readcount = 0;
                msg.bit = 0;
                msg.readonly = qtrue;
                msg.overflowed = qfalse;

                return tcpevents[i].tcpevent(from, &msg, socketfd, connectionId);
            }
        }

        Com_PrintError("NET_TCPPacketEvent: Bad serviceId: %d\n", serviceId);
        return qtrue; //Close connection
}

void NET_TCPAddEventType(qboolean (*tcpevent)(netadr_t* from, msg_t* msg, int socketfd, int connectionId), int serviceId)
{
    int i;

    for(i = 0; i < MAX_TCPEVENTS; i++)
    {
        if(tcpevents[i].tcpevent ==  tcpevent)
        {
            Com_Error(ERR_FATAL, "NET_TCPAddEventType: Attempt to add an already defined redirect function twice.");
            return;
        }

        if(tcpevents[i].tcpevent == NULL)
        {
            tcpevents[i].tcpevent = tcpevent;
            return;
        }
    }
    Com_Error(ERR_FATAL, "NET_TCPAddEventType: Out of redirect handles. Increase MAX_TCPEVENTS to add more redirect destinations");
}
