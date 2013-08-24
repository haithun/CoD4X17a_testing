#include "plugin_handler.h"

/*=========================================*
 *                                         *
 *        Plugin Handler's exports         *
 *                                         *
 *   Functions in this file are available  *
 *           for use in plugins.           *
 *                                         *
 *=========================================*/


P_P_F void *Plugin_Malloc(size_t size)
{
    volatile int pID;
    //Identify the calling plugin
    pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugins: Error! Tried allocating memory for unknown plugin!\n");
        return NULL;
    }
    if(pluginFunctions.plugins[pID].enabled==qfalse){
        Com_Printf("^1WARNING^7: Tried allocating memory for a disabled plugin!\n");

    }
    return PHandler_Malloc(pID,size);

}
P_P_F void Plugin_Free(void *ptr)
{
    //Identify the calling plugin
    volatile int pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugins: Error! Tried freeing memory for unknown plugin!\n");
        return;
    }
    PHandler_Free(pID,ptr);
}
P_P_F void Plugin_Error(int code, char *string)
{
    volatile int pID = PHandler_CallerID();
    if(pID<0){
        Com_PrintError("Plugin Error called from unknown plugin!\n");
        return;
    }
    PHandler_Error(pID,code,string);
}
P_P_F int Plugin_ClientToSlot(client_t *client)
{
    return client-svs.clients;
}

P_P_F clientScoreboard_t Plugin_GetClientScoreboard(int clientNum)
{
    return level.clients[clientNum].pers.scoreboard;
}
P_P_F int Plugin_Cmd_GetInvokerUid()
{
    return SV_RemoteCmdGetInvokerUid();
}
P_P_F int Plugin_GetPlayerUid(int slot)
{
    return svs.clients[slot].uid;
}
P_P_F int Plugin_GetSlotCount()
{
    return sv_maxclients->integer;
}
P_P_F qboolean Plugin_IsSvRunning()
{
    if(com_sv_running)
        return com_sv_running->boolean;
    else
        return qfalse;
}
P_P_F char *Plugin_GetPlayerName(int slot)
{
    return svs.clients[slot].name;
}
P_P_F void Plugin_AddCommand(char *name, xcommand_t xcommand, int power)
{
    volatile int pID;
    pID = PHandler_CallerID();
    if(pID>=MAX_PLUGINS){
        Com_PrintError("Tried adding a command for a plugin with non existent pID. pID supplied: %d.\n",pID);
        return;
    }else if(pID<0){
        Com_PrintError("Plugin_AddCommand called from not within a plugin or from a disabled plugin!\n");
        return;
    }
    if(!pluginFunctions.plugins[pID].loaded){
        Com_PrintError("Tried adding a command for not loaded plugin! PID: %d.\n",pID);
    }
    Com_DPrintf("Adding a plugin command for plugin %d, command name: %s.\n",pID,name);
    Cmd_AddCommand(name,PHandler_CmdExecute_f);
    Cmd_SetPower(name, power);
    pluginFunctions.plugins[pID].cmd[pluginFunctions.plugins[pID].cmds].xcommand = xcommand;
    strcpy(pluginFunctions.plugins[pID].cmd[pluginFunctions.plugins[pID].cmds++].name,name);
    Com_DPrintf("Command added.\n");
   // pluginFunctions.plugins[pID].


}
P_P_F qboolean Plugin_TcpConnect( int connection, const char* remote)
{
    int pID;
    //Identify the calling plugin
    pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugins: Error! Tried open a TCP-Connection for unknown plugin!\n");
        return qfalse;
    }
    if(pluginFunctions.plugins[pID].enabled==qfalse){
        Com_Printf("^1WARNING^7: Tried open a TCP-Connection for a disabled plugin!\n");
        return qfalse;
    }
    if(connection >= PLUGIN_MAX_SOCKETS || connection < 0){
        Com_PrintError("Plugin_TcpConnect: Second argument can only be a value inside the range: 0...%d plugin ID: #%d\n", PLUGIN_MAX_SOCKETS);
        return qfalse;
    }
    return PHandler_TcpConnect(pID, remote, connection);

}

P_P_F int Plugin_TcpGetData(int connection, void* buf, int size)
{
    int pID;
    //Identify the calling plugin
    pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugin_TcpGetData: Error! Tried get TCP data for unknown plugin!\n");
        return -1;
    }
    if(pluginFunctions.plugins[pID].enabled==qfalse){
        Com_Printf("^1WARNING^7: Plugin_TcpGetData: Tried get TCP data for a disabled plugin!\n");
        return -1;
    }
    if(connection >= PLUGIN_MAX_SOCKETS || connection < 0){
        Com_PrintError("Plugin_TcpGetData: First argument can only be a value inside the range: 0...%d plugin ID: #%d\n", PLUGIN_MAX_SOCKETS, pID);
        return -1;
    }
    if(buf == NULL){
        Com_PrintError("Plugin_TcpGetData: Third argument can not be a NULL-Pointer for plugin ID: #%d\n", pID);
        return -1;
    }

    return PHandler_TcpGetData(pID, connection, buf, size);
}


P_P_F qboolean Plugin_TcpSendData(int connection, void* data, int len)
{


    int pID;
    //Identify the calling plugin
    pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugin_TcpSendData: Error! Tried get TCP data for unknown plugin!\n");
        return qfalse;
    }
    if(pluginFunctions.plugins[pID].enabled==qfalse){
        Com_Printf("^1WARNING^7: Plugin_TcpSendData: Tried get TCP data for a disabled plugin!\n");
        return qfalse;
    }
    if(connection >= PLUGIN_MAX_SOCKETS || connection < 0){
        Com_PrintError("Plugin_TcpSendData: Second argument can only be a value inside the range: 0...%d plugin ID: #%d\n", PLUGIN_MAX_SOCKETS, pID);
        return qfalse;
    }
    if(data == NULL){
        Com_PrintError("Plugin_TcpSendData: Second argument can not be a NULL-Pointer for plugin ID: #%d\n", pID);
        return qfalse;
    }

    return PHandler_TcpSendData(pID, connection, data, len);
}

P_P_F void Plugin_TcpCloseConnection(int connection)
{
    int pID;
    //Identify the calling plugin
    pID = PHandler_CallerID();
    if(pID<0){
        Com_Printf("Plugin_TcpCloseConnection: Error! Tried get close a connection for unknown plugin!\n");
        return;
    }
    if(pluginFunctions.plugins[pID].enabled==qfalse){
        Com_Printf("^1WARNING^7: Plugin_TcpCloseConnection: Tried to close a connection for a disabled plugin!\n");
        return;
    }
    if(connection >= PLUGIN_MAX_SOCKETS || connection < 0){
        Com_PrintError("Plugin_TcpCloseConnection: Second argument can only be a value inside the range: 0...%d plugin ID: #%d\n", PLUGIN_MAX_SOCKETS, pID);
        return;
    }
    PHandler_TcpCloseConnection(pID, connection);
}

P_P_F qboolean Plugin_UdpSendData(netadr_t* to, void* data, int len)
{
    int pID;

    if(to == NULL){
        pID = PHandler_CallerID();
        Com_PrintError("Plugin_UdpSendData: First argument can not be a NULL-Pointer for plugin ID: #%d\n", pID);
        return qfalse;
    }

    if(data == NULL){
        pID = PHandler_CallerID();
        Com_PrintError("Plugin_UdpSendData: First argument can not be a NULL-Pointer for plugin ID: #%d\n", pID);
        return qfalse;
    }
    return Sys_SendPacket( len, data, to);
}

P_P_F void Plugin_ServerPacketEvent(netadr_t* to, void* data, int len)
{
    msg_t msg;
    msg.data = data;
    msg.cursize = len;
    msg.maxsize = len;
    msg.bit = 0;
    msg.overflowed = qfalse;
    msg.readonly = qtrue;

    SV_PacketEvent( to, &msg );
}

P_P_F void Plugin_SetPlayerUID(unsigned int clientslot, unsigned int uid)
{
    client_t *cl;
    int PID = PHandler_CallerID();
    if(!psvs.useuids){
        PHandler_Error(PID,P_ERROR_DISABLE, "Plugin tried to set UID while server doesn't work with UIDs\n");
    }
    if(clientslot > sv_maxclients->integer)
    {
        PHandler_Error(PID,P_ERROR_DISABLE, va("Plugin tried to set UID for bad client: %d\n", clientslot));
    }
    cl = &svs.clients[clientslot];
    cl->uid = uid;
}

P_P_F unsigned int Plugin_GetPlayerUID(unsigned int clientslot)
{
    client_t *cl;
    int PID = PHandler_CallerID();
    if(!psvs.useuids){
        PHandler_Error(PID,P_ERROR_TERMINATE, "Plugin tried to get UID while server don't work with UIDs\n");
    }
    if(clientslot > sv_maxclients->integer)
    {
        PHandler_Error(PID,P_ERROR_DISABLE, va("Plugin tried to get UID for bad client: %d\n", clientslot));
    }
    cl = &svs.clients[clientslot];
    return cl->uid;
}

P_P_F const char* Plugin_GetPlayerGUID(unsigned int clientslot)
{
    client_t *cl;
    int PID = PHandler_CallerID();
    if(clientslot > sv_maxclients->integer)
    {
        PHandler_Error(PID,P_ERROR_DISABLE, va("Plugin tried to get GUID for bad client: %d\n", clientslot));
    }
    cl = &svs.clients[clientslot];
    return cl->pbguid;
}

P_P_F void Plugin_SetPlayerGUID(unsigned int clientslot, const char* guid)
{
    client_t *cl;
    int PID = PHandler_CallerID();
    if(clientslot > sv_maxclients->integer)
    {
        PHandler_Error(PID,P_ERROR_DISABLE, va("Plugin tried to get GUID for bad client: %d\n", clientslot));
    }
    cl = &svs.clients[clientslot];
    Q_strncpyz(cl->pbguid, guid, sizeof(cl->pbguid));
}

P_P_F void Plugin_SetPlayerNoPB(unsigned int clientslot)
{
    client_t *cl;
    int PID = PHandler_CallerID();
    if(clientslot > sv_maxclients->integer)
    {
        PHandler_Error(PID,P_ERROR_DISABLE, va("Plugin tried to set No PunkBuster flag for bad client: %d\n", clientslot));
    }
    cl = &svs.clients[clientslot];
    cl->noPb = qtrue;
}

P_P_F int Plugin_DoesServerUseUids(void)
{
    return psvs.useuids;
}

P_P_F void Plugin_SetServerToUseUids(int useuids)
{
    psvs.useuids = useuids;
}

P_P_F int Plugin_GetLevelTime(void)
{
    return level.time;
}

P_P_F int Plugin_GetServerTime(void)
{
    return svs.time;
}
