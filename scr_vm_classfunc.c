

DLL_PUBLIC void ClientScr_SetSessionTeam(gclient_t* gcl, client_fields_t* gfl)
{

    short index;
    int cid;

    if((void*)gcl - (void*)level.clients >= MAX_CLIENTS * sizeof(gclient_t))
    {
        Scr_Error("Client is not pointing to the level.clients array.");
        return;
    }

    index = Scr_GetConstString(0);

    if(index == stringIndex.axis)
        gcl->sess.sessionTeam = TEAM_RED;
    else if(index == stringIndex.allies)
        gcl->sess.sessionTeam = TEAM_BLUE;
    else if(index == stringIndex.spectator)
        gcl->sess.sessionTeam = TEAM_SPECTATOR;

    else if(index == stringIndex.none)
        gcl->sess.sessionTeam = TEAM_FREE;

    else{
        Scr_Error(va("'%s' is an illegal sessionteam string. Must be allies, axis, none, or spectator.", SL_ConvertToString(index)));
        return;
    }

    cid = gcl - level.clients;

    ClientUserinfoChanged( cid );

    HL2Rcon_EventClientEnterTeam( cid, gcl->sess.sessionTeam );

}