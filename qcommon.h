#ifndef __QCOMMON_H__
#define __QCOMMON_H__

#include <time.h>
#include "cvar.h"

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
int QDECL Com_AddTimedEvent( int delay, void *function, unsigned int argcount, ...);

void Com_RandomBytes( byte *string, int len );
int Com_HashKey( char *string, int maxlen );
void Com_Quit_f( void );
unsigned int Com_GetFrameTime(void);
unsigned long long Com_GetUsecFrameTime(void);

extern unsigned long long com_frameTime;
extern unsigned long long com_uFrameTime;
extern cvar_t* com_dedicated;
extern cvar_t* com_timescale;

int Com_IsDeveloper();


#endif