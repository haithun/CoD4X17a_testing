#ifndef __SYS_MAIN_H__
#define __SYS_MAIN_H__

#include "q_shared.h"

int Sys_Milliseconds( void );
void Sys_TimerInit( void );
unsigned long long Sys_Microseconds( void );
int Sys_Seconds( void );
void Sys_Quit( void );
void Sys_Print( const char *msg );
void Sys_SigHandler( int signal );
void Sys_TermProcess(int signal);
char *Sys_ConsoleInput(void);
void Sys_AnsiColorPrint( const char *msg );
void Sys_PrintBinVersion( const char* name );
void Sys_ParseArgs( int argc, char* argv[] );
__cdecl DLL_PUBLIC void QDECL Sys_Error( const char *fmt, ... );
void Sys_SetBinaryPath(const char *path);
char *Sys_BinaryPath(void);
void Sys_SetDefaultInstallPath(const char *path);
char *Sys_DefaultInstallPath(void);
char *Sys_DefaultAppPath(void);
//sys_unix.c
qboolean Sys_RandomBytes( byte *string, int len );
void Sys_DoStartProcess( char *cmdline );
const char *Sys_Dirname( char *path );
char *Sys_Cwd( void );
void Sys_InitCrashDumps();
qboolean Sys_MemoryProtectWrite(void* startoffset, int len);
qboolean Sys_MemoryProtectExec(void* startoffset, int len);
qboolean Sys_MemoryProtectReadonly(void* startoffset, int len);
const char *Sys_DefaultHomePath(void);
const char *Sys_TempPath( void );

#endif
