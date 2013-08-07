#include "declarations.h"


int GetFunctions(mainFunctions_t mF){
	Com_Printf = mF.Com_Printf;
	Com_DPrintf = mF.Com_DPrintf;
	Com_PrintWarning = mF.Com_PrintWarning;
	Com_PrintError = mF.Com_PrintError;
	Cmd_Argv = mF.Cmd_Argv;
	Cmd_Argc = mF.Cmd_Argc;
	FS_SV_FOpenFileRead = mF.FS_SV_FOpenFileRead;
	FS_SV_FOpenFileWrite = mF.FS_SV_FOpenFileWrite;
	FS_Read = mF.FS_Read;
	FS_ReadLine = mF.FS_ReadLine;
	FS_Write = mF.FS_Write;
	FS_FCloseFile = mF.FS_FCloseFile;
	Com_ParseGetToken = mF.Com_ParseGetToken;
	Com_ParseTokenLength = mF.Com_ParseTokenLength;
	Cvar_VariableStringBuffer = mF.Cvar_VariableStringBuffer;
	Cvar_VariableValue = mF.Cvar_VariableValue;
	Cvar_VariableIntegerValue = mF.Cvar_VariableIntegerValue;
	Cvar_VariableString = mF.Cvar_VariableString;
	Sys_Milliseconds = mF.Sys_Milliseconds;
	Plugin_GetClientScoreboard = mF.Plugin_GetClientScoreboard;
	Plugin_Cmd_GetInvokerUid = mF.Plugin_Cmd_GetInvokerUid;
	Plugin_GetPlayerUid = mF.Plugin_GetPlayerUid;
	Plugin_GetSlotCount = mF.Plugin_GetSlotCount;
	Plugin_IsSvRunning = mF.Plugin_IsSvRunning;
	Plugin_ChatPrintf = mF.Plugin_ChatPrintf;
	Plugin_BoldPrintf = mF.Plugin_BoldPrintf;
	Plugin_GetPlayerName = mF.Plugin_GetPlayerName;
	Plugin_AddCommand = mF.Plugin_AddCommand;
	Plugin_Malloc = mF.Plugin_Malloc;
	Plugin_Free = mF.Plugin_Free;
	Plugin_Error = mF.Plugin_Error;
	Plugin_GetLevelTime = mF.Plugin_GetLevelTime;
	Plugin_GetServerTime = mF.Plugin_GetServerTime;
	Plugin_TcpConnect = mF.Plugin_TcpConnect;
	Plugin_TcpGetData = mF.Plugin_TcpGetData;
	Plugin_TcpSendData = mF.Plugin_TcpSendData;
	Plugin_TcpCloseConnection = mF.Plugin_TcpCloseConnection;
	Plugin_UdpSendData = mF.Plugin_UdpSendData;
	Plugin_ServerPacketEvent = mF.Plugin_ServerPacketEvent;
	Plugin_SetPlayerUID = mF.Plugin_SetPlayerUID;
	Plugin_GetPlayerUID = mF.Plugin_GetPlayerUID;
	Plugin_GetPlayerGUID = mF.Plugin_GetPlayerGUID;
	Plugin_SetPlayerGUID = mF.Plugin_SetPlayerGUID;
	Plugin_SetPlayerNoPB = mF.Plugin_SetPlayerNoPB;
	Plugin_DoesServerUseUids = mF.Plugin_DoesServerUseUids;
	Plugin_SetServerToUseUids = mF.Plugin_SetServerToUseUids;
	NET_StringToAdr = mF.NET_StringToAdr;
	Cvar_RegisterString = mF.Cvar_RegisterString;
	Cvar_RegisterBool = mF.Cvar_RegisterBool;
	Cvar_RegisterInt = mF.Cvar_RegisterInt;
	Cvar_RegisterEnum = mF.Cvar_RegisterEnum;
	Cvar_RegisterFloat = mF.Cvar_RegisterFloat;
	Cvar_SetInt = mF.Cvar_SetInt;
	Cvar_SetBool = mF.Cvar_SetBool;
	Cvar_SetString = mF.Cvar_SetString;
	Cvar_SetFloat = mF.Cvar_SetFloat;

	return 1;
}




