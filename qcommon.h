#ifndef __QCOMMON_H__
#define __QCOMMON_H__

#include <time.h>

void __cdecl Com_InitParse( void );
void __cdecl Com_InitCvars(void);
void __cdecl Com_Restart(void);
void __cdecl Com_WriteConfig_f(void);
void __cdecl Com_WriteDefaults_f(void);
void __cdecl Com_DvarDump(int, int);
void __cdecl Com_Close(void);
void QDECL Com_Error( int level, const char *error, ...);
int __cdecl Com_Filter( char* filter, char *name, int casesensitive);

void Com_Frame(void);
void Com_Init(char* cmdline);
void Com_UpdateRealtime();
time_t Com_GetRealtime();

#endif