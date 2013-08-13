typedef void (__cdecl *tLiveStorage_Init)(void);
tLiveStorage_Init LiveStorage_Init = (tLiveStorage_Init)(0x81d77ce);

typedef void (__cdecl *tSV_StringUsage_f)(void);
tSV_StringUsage_f SV_StringUsage_f = (tSV_StringUsage_f)(0x814cc7e);

typedef void (__cdecl *tSV_ScriptUsage_f)(void);
tSV_ScriptUsage_f SV_ScriptUsage_f = (tSV_ScriptUsage_f)(0x81557c4);

/*
//Netchan and MSG

typedef char* (__cdecl *tMSG_Init)( msg_t *buf, byte *data, int length);
tMSG_Init MSG_Init = (tMSG_Init)(0x8131200);

typedef char* (__cdecl *tMSG_ReadString)( msg_t *msg);
tMSG_ReadString MSG_ReadString = (tMSG_ReadString)(0x81310d6);

typedef char* (__cdecl *tMSG_ReadStringLine)( msg_t *msg);
tMSG_ReadStringLine MSG_ReadStringLine = (tMSG_ReadStringLine)(0x8130f92);

typedef int (__cdecl *tMSG_ReadLong)( msg_t *msg);
tMSG_ReadLong MSG_ReadLong = (tMSG_ReadLong)(0x813099a);

typedef int (__cdecl *tMSG_ReadShort)( msg_t *msg);
tMSG_ReadShort MSG_ReadShort = (tMSG_ReadShort)(0x8130920);

typedef int (__cdecl *tMSG_ReadByte)( msg_t *msg);
tMSG_ReadByte MSG_ReadByte = (tMSG_ReadByte)(0x81308d8);

typedef int (__cdecl *tMSG_ReadData)( msg_t *msg, const void* data, int length);
tMSG_ReadData MSG_ReadData = (tMSG_ReadData)(0x8130bc2);

typedef void (__cdecl *tMSG_BeginReading)( msg_t *msg);
tMSG_BeginReading MSG_BeginReading = (tMSG_BeginReading)(0x81305c6);

typedef void (__cdecl *tMSG_WriteLong)( msg_t *msg, int c);
tMSG_WriteLong MSG_WriteLong = (tMSG_WriteLong)(0x81308aa);

typedef void (__cdecl *tMSG_WriteShort)( msg_t *msg, int c);
tMSG_WriteShort MSG_WriteShort = (tMSG_WriteShort)(0x813087c);

typedef void (__cdecl *tMSG_WriteData)( msg_t *msg, const void* data, int length);
tMSG_WriteData MSG_WriteData = (tMSG_WriteData)(0x8130c88);
*/

typedef int (__cdecl *tMSG_ReadBits)( msg_t *msg, int numBits);
tMSG_ReadBits MSG_ReadBits = (tMSG_ReadBits)(0x8130730);

typedef int (__cdecl *tGetMinBitCount)( unsigned int number );
tGetMinBitCount GetMinBitCount = (tGetMinBitCount)(0x8130500);

typedef void (__cdecl *tMSG_WriteDeltaClient)(snapshotInfo_t* , msg_t* , int , clientState_s* , clientState_s*, int);
tMSG_WriteDeltaClient MSG_WriteDeltaClient = (tMSG_WriteDeltaClient)(0x813f49c);

typedef void (__regparm3 *tMSG_WriteDeltaField)(snapshotInfo_t* , msg_t* , int, unsigned const char*, unsigned const char*, const void* netfield, int, unsigned char);
tMSG_WriteDeltaField MSG_WriteDeltaField = (tMSG_WriteDeltaField)(0x813e22a);

typedef void (__cdecl *tMSG_WriteDeltaPlayerstate)(snapshotInfo_t* , msg_t* , int , playerState_t* , playerState_t*);
tMSG_WriteDeltaPlayerstate MSG_WriteDeltaPlayerstate = (tMSG_WriteDeltaPlayerstate)(0x813f5a6);

typedef void (__cdecl *tMSG_WriteEntityIndex)(snapshotInfo_t*, msg_t*, int, int);
tMSG_WriteEntityIndex MSG_WriteEntityIndex = (tMSG_WriteEntityIndex)(0x813de54);

typedef void (__cdecl *tMSG_WriteBit1)(msg_t*);
tMSG_WriteBit1 MSG_WriteBit1 = (tMSG_WriteBit1)(0x81306dc);

typedef void (__cdecl *tMSG_WriteBits)(msg_t*, int bits, int bitcount);
tMSG_WriteBits MSG_WriteBits = (tMSG_WriteBits)(0x813061c);

typedef void (__cdecl *tMSG_ReadDeltaUsercmdKey)( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
tMSG_ReadDeltaUsercmdKey MSG_ReadDeltaUsercmdKey = (tMSG_ReadDeltaUsercmdKey)(0x81392ae);

typedef void (__cdecl *tMSG_SetDefaultUserCmd)( playerState_t *ps, usercmd_t *ucmd );
tMSG_SetDefaultUserCmd MSG_SetDefaultUserCmd = (tMSG_SetDefaultUserCmd)(0x8130ad0);

//typedef int (__cdecl *tMSG_ReadBitsCompress)( const byte *datasrc, byte *buffdest, int bytecount);
//tMSG_ReadBitsCompress MSG_ReadBitsCompress = (tMSG_ReadBitsCompress)(0x8130edc);

typedef int (__cdecl *tMSG_WriteBitsCompress)( char dummy, const byte *datasrc, byte *buffdest, int bytecount);
tMSG_WriteBitsCompress MSG_WriteBitsCompress = (tMSG_WriteBitsCompress)(0x8130f3c);

typedef void (__cdecl *tSV_BeginClientSnapshot)( client_t *cl, msg_t* msg);
tSV_BeginClientSnapshot SV_BeginClientSnapshot = (tSV_BeginClientSnapshot)(0x817a32e);

typedef void (__cdecl *tSV_EndClientSnapshot)( client_t *cl, msg_t* msg);
tSV_EndClientSnapshot SV_EndClientSnapshot = (tSV_EndClientSnapshot)(0x817b488);

typedef void (__cdecl *tSV_ClientThink)( client_t *cl, usercmd_t * );
tSV_ClientThink SV_ClientThink = (tSV_ClientThink)(0x816df7a);
/*
typedef void (__cdecl *tSV_ExecuteClientMessage)(client_t*, msg_t*);
tSV_ExecuteClientMessage SV_ExecuteClientMessage = (tSV_ExecuteClientMessage)(0x8172990);
*/
typedef void (__cdecl *tSV_SpawnServer)(const char* levelname);
tSV_SpawnServer SV_SpawnServer = (tSV_SpawnServer)(0x8174a68);

typedef void (__cdecl *tSV_SetGametype)( void );
tSV_SetGametype SV_SetGametype = (tSV_SetGametype)(0x817c72c);

typedef void (__cdecl *tSV_InitCvars)( void );
tSV_InitCvars SV_InitCvars = (tSV_InitCvars)(0x819e66c);

typedef void (__cdecl *tSV_RestartGameProgs)( int savepersist );
tSV_RestartGameProgs SV_RestartGameProgs = (tSV_RestartGameProgs)(0x817c68a);

typedef void (__cdecl *tSV_ResetSekeletonCache)(void);
tSV_ResetSekeletonCache SV_ResetSekeletonCache = (tSV_ResetSekeletonCache)(0x817c602);

typedef void (__cdecl *tSV_PreFrame)(void);
tSV_PreFrame SV_PreFrame = (tSV_PreFrame)(0x8177534);

typedef void (__cdecl *tSV_SendClientMessages)(void);
tSV_SendClientMessages SV_SendClientMessages = (tSV_SendClientMessages)(0x817b71e);

/*
typedef void (__cdecl *tSV_Shutdown)(const char* reason);
tSV_Shutdown SV_Shutdown = (tSV_Shutdown)(0x817452e);
*/

typedef void (__cdecl *tSV_SetServerStaticHeader)(void);
tSV_SetServerStaticHeader SV_SetServerStaticHeader = (tSV_SetServerStaticHeader)(0x8178fd4);

typedef void (__cdecl *tSV_ShutdownGameProgs)(void);
tSV_ShutdownGameProgs SV_ShutdownGameProgs = (tSV_ShutdownGameProgs)(0x817c818);

typedef void (__cdecl *tSV_FreeClients)(void);
tSV_FreeClients SV_FreeClients = (tSV_FreeClients)(0x81720c6);

/*typedef void (__cdecl *tG_LogPrintf)( const char *text);
tG_LogPrintf G_LogPrintf = (tG_LogPrintf)(0x80b43c4);
*/

typedef void (__cdecl *tG_RunFrame)( int time );
tG_RunFrame G_RunFrame = (tG_RunFrame)(0x80b61fc);

typedef qboolean (__regparm1 *tIs_Banned)(const char* guid);
tIs_Banned Is_Banned = (tIs_Banned)(0x816ece4);

typedef void (__cdecl *tSV_GameSendServerCommand)(int clientnum, int svscmd_type, const char *text);
tSV_GameSendServerCommand SV_GameSendServerCommand = (tSV_GameSendServerCommand)(0x817ce42);

typedef void (__cdecl *tSV_SetConfigstring)(int index, const char *text);
tSV_SetConfigstring SV_SetConfigstring = (tSV_SetConfigstring)(0x8173fda);

typedef void (__cdecl *tSV_AddServerCommand_old)(client_t *client, int unkownzeroorone, const char *);
tSV_AddServerCommand_old SV_AddServerCommand_old = (tSV_AddServerCommand_old)(0x817664c);

//typedef void (__cdecl *tSV_DropClient)(client_t *drop, const char* reason);
//tSV_DropClient SV_DropClient = (tSV_DropClient)(0x8170a26);

typedef void (__cdecl *tSV_FreeClient)(client_t* drop);
tSV_FreeClient SV_FreeClient = (tSV_FreeClient)(0x81708bc);

typedef qboolean (__cdecl *tSV_BanClient)( unsigned int *clientstatusptr);
tSV_BanClient SV_BanClient = (tSV_BanClient)(0x8171410);

/*
typedef qboolean (__regparm3 *tSV_ClientCommand)( client_t*, msg_t*, int);
tSV_ClientCommand SV_ClientCommand = (tSV_ClientCommand)(0x8170d22);
*/

typedef void (__cdecl *tClientCommand)( int );
tClientCommand ClientCommand = (tClientCommand)(0x80b070c);

/*
typedef void (__cdecl *tClientUserinfoChanged)( int );
tClientUserinfoChanged ClientUserinfoChanged = (tClientUserinfoChanged)(0x80a8068);
*/

//typedef sharedEntity_t* (__cdecl *tSV_GentityNum)(int clnum);
//tSV_GentityNum SV_GentityNum = (tSV_GentityNum)(0x817c586);

typedef void (__cdecl *tSV_FreeClientScriptId)(client_t *cl);
tSV_FreeClientScriptId SV_FreeClientScriptId = (tSV_FreeClientScriptId)(0x8175c5e);

//typedef void (__cdecl *tSV_SendClientGameState)(client_t *cl);
//tSV_SendClientGameState SV_SendClientGameState = (tSV_SendClientGameState)(0x816e1ba);

/*typedef void (__cdecl *tSV_ClientEnterWorld)(client_t *cl, usercmd_t* ucmd);
tSV_ClientEnterWorld SV_ClientEnterWorld = (tSV_ClientEnterWorld)(0x816e136);*/

//typedef void (__cdecl *tSV_UserinfoChanged)(client_t *cl);
//tSV_UserinfoChanged SV_UserinfoChanged = (tSV_UserinfoChanged)(0x81703d6);

typedef void (__cdecl *tSV_LinkEntity)(gentity_t*);
tSV_LinkEntity SV_LinkEntity = (tSV_LinkEntity)(0x817eb20);

typedef void (__cdecl *tSV_UnlinkEntity)(gentity_t*);
tSV_UnlinkEntity SV_UnlinkEntity = (tSV_UnlinkEntity)(0x817d5e0);

//Check for ban or PunkBuster is turned on at client

typedef const char* (__cdecl *tClientConnect)(int clnum, short clscriptid);  //Something simular to VM_Call
tClientConnect ClientConnect = (tClientConnect)(0x80a83d4);

typedef void (__regparm3 *tG_SayTo)(gentity_t *ent, gentity_t *other, int mode, int color, const char* teamname, const char* name, const char *message);
tG_SayTo G_SayTo = (tG_SayTo)(0x80ae6f4);

/*typedef cvar_t* (__cdecl *tCvar_SetBool)(cvar_bool_t *var_name,int var_value);
tCvar_SetBool Cvar_SetBool = (tCvar_SetBool)(0x81a20c4); Does not work like expected !
*/

//G_Say 80ae962

typedef void (__cdecl *tClientBegin)( int clientNum );
tClientBegin ClientBegin = (tClientBegin)(0x80a7700);

typedef void (__cdecl *tMSG_WriteReliableCommandToBuffer)( const char* source, char* destination,int length);
tMSG_WriteReliableCommandToBuffer MSG_WriteReliableCommandToBuffer = (tMSG_WriteReliableCommandToBuffer)(0x813e162);

/*
typedef void (__cdecl *tHuff_offsetReceive)( node_t *node, int *ch, byte *fin, int *offset );
tHuff_offsetReceive Huff_offsetReceive = (tHuff_offsetReceive)(0x812a0a2);*/
/*
typedef void (__cdecl *tHuff_offsetReceive)( node_t *node, int *ch, byte *fin, int *offset );
tHuff_offsetReceive Huff_offsetReceive = (tHuff_offsetReceive)(0x812a0a2);*/

int MSG_WriteBitsNoCompress( int d, byte* src, byte* dst , int size){
	Com_Memcpy(dst, src, size);
	return size;
}


typedef void (__cdecl *tStopFollowing)( gentity_t* ent );
tStopFollowing StopFollowing = (tStopFollowing)(0x80af24c);

typedef unsigned int (__cdecl *tScr_LoadScript_o)(const char* scriptname, void *precache, int iarg_02);
tScr_LoadScript_o Scr_LoadScript_o = (tScr_LoadScript_o)(0x814bef0);
/*
typedef char* (__cdecl *tSL_ConvertToString)(unsigned int index);
tSL_ConvertToString SL_ConvertToString = (tSL_ConvertToString)(0x8150340);
*/