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
void Sys_EnterCriticalSection(int section);
void Sys_LeaveCriticalSection(int section);