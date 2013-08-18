#ifndef __MISC_H__
#define __MISC_H__

#include "q_shared.h"
#include "player.h"

#define STRBUFFBASEPTR_ADDR 0x897d780


void __cdecl Swap_Init(void);
void __cdecl CSS_InitConstantConfigStrings(void);
void __cdecl Con_InitChannels(void);
void __cdecl SEH_UpdateLanguageInfo(void);
void __cdecl SetAnimCheck(int);
qboolean __cdecl BG_IsWeaponValid( playerState_t *ps, unsigned int index);
qboolean __cdecl SEH_StringEd_GetString( const char* input );
void __cdecl DObjInit(void);

void __cdecl SL_Init(void);
void __cdecl SL_RemoveRefToString( unsigned int );
char* SL_ConvertToString(unsigned int index);

#endif

/*
void __cdecl HECmd_SetText)( scr_entref_t );
tHECmd_SetText HECmd_SetText = (tHECmd_SetText)(0x808f7f6);
*/

