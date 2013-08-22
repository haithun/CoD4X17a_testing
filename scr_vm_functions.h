#ifndef __SCR_VM_FUNCTIONS_H__
#define __SCR_VM_FUNCTIONS_H__

#include "scr_vm.h"
#include "g_sv_shared.h"

#ifndef scr_entref_t
typedef int scr_entref_t;
#endif

void PlayerCmd_GetUid(scr_entref_t arg);
void PlayerCmd_GetUserinfo(scr_entref_t arg);
void PlayerCmd_GetPing(scr_entref_t arg);
void PlayerCmd_SetGravity(scr_entref_t arg);
void PlayerCmd_SetJumpHeight(scr_entref_t arg);
void PlayerCmd_SetMoveSpeed(scr_entref_t arg);
void PlayerCmd_GetGeoLocation(scr_entref_t arg);
void GScr_StrTokByPixLen();
void GScr_StrTokByLen();
void GScr_StrPixLen();
void GScr_StrColorStrip();
void GScr_StrRepl();
void GScr_CopyString();
void GScr_GetRealTime();
void GScr_TimeToString();
void GScr_SHA256();
void GScr_CbufAddText();
void GScr_FS_FOpen();
void GScr_FS_FClose();
void GScr_FS_FCloseAll();
void GScr_FS_TestFile();
void GScr_FS_ReadLine();
void GScr_FS_WriteLine();
void GScr_FS_Remove();
void GScr_SpawnBot();
void GScr_RemoveAllBots();
void GScr_RemoveBot();
void GScr_KickClient();
void GScr_BanClient();
void PlayerCmd_spawn(scr_entref_t arg);
void GScr_NewHudElem();
void GScr_NewClientHudElem();
void HECmd_SetText(scr_entref_t entnum);

__cdecl void ClientScr_SetSessionTeam(gclient_t* gcl, client_fields_t* gfl);

#endif