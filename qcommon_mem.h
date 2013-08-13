#ifndef __QCOMMON_MEM_H__
#define __QCOMMON_MEM_H__

#include <stdlib.h>
#include "q_shared.h"

void __cdecl Com_InitHunkMemory(void);
void __cdecl Hunk_InitDebugMemory(void);
void __cdecl Hunk_ClearTempMemory(void);
void __cdecl Hunk_ClearTempMemoryHigh(void);
void* __cdecl Hunk_AllocateTempMemory(int size);
void __cdecl Hunk_FreeTempMemory(void *buffer);
void* __cdecl Z_Malloc( int size);
void __cdecl Mem_Init(void);
void __cdecl Mem_BeginAlloc(const char*, qboolean);
void __cdecl Mem_EndAlloc(const char*, int);


#define Z_Free free

#endif
