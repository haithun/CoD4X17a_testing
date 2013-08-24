#ifndef __SYS_THREAD_H__
#define __SYS_THREAD_H__

void __cdecl Sys_EnterCriticalSection(int section);
void __cdecl Sys_LeaveCriticalSection(int section);
void __cdecl Sys_ThreadInit( void );
void __cdecl Sys_ThreadMain( void );
qboolean __cdecl Sys_IsMainThread( void );
void Com_InitThreadData(void);
const void* __cdecl Sys_GetValue(int key);
void __cdecl Sys_SetValue(int key, const void* value);

#endif