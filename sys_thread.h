#ifndef __SYS_THREAD_H__
#define __SYS_THREAD_H__

void __cdecl Sys_EnterCriticalSection(int section);
void __cdecl Sys_LeaveCriticalSection(int section);
void __cdecl Sys_ThreadInit( void );
void __cdecl Sys_ThreadMain( void );
void __cdecl Sys_IsMainThread( void );


#endif