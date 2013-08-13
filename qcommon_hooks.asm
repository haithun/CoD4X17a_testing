SECTION .text

global Com_Error
Com_Error:
    jmp 0x812257c

global Com_InitParse
Com_InitParse:
    jmp 0x81a7a78

global Com_Restart
Com_Restart:
    jmp 0x8123586

global Com_InitHunkMemory
Com_InitHunkMemory:
    jmp 0x8197174

global Hunk_InitDebugMemory
Hunk_InitDebugMemory:
    jmp 0x819752a

global Com_WriteConfig_f
Com_WriteConfig_f:
    jmp 0x8122e82

global Com_WriteDefaults_f
Com_WriteDefaults_f:
    jmp 0x8123bae

global Com_ErrorCleanup
Com_ErrorCleanup:
    jmp 0x8123c86

global Hunk_ClearTempMemory
Hunk_ClearTempMemory:
    jmp 0x81968a8

global Hunk_ClearTempMemoryHigh
Hunk_ClearTempMemoryHigh:
    jmp 0x81968b8

global Com_Close
Com_Close:
    jmp 0x8121b82

global Hunk_AllocateTempMemory
Hunk_AllocateTempMemory:
    jmp 0x8196fea

global Hunk_FreeTempMemory
Hunk_FreeTempMemory:
    jmp 0x81969d4

global Z_Malloc
Z_Malloc:
    jmp 0x8196d6e

global Com_Filter
Com_Filter:
    jmp 0x819837e

global Mem_Init
Mem_Init:
    jmp 0x81a75e6

global Mem_BeginAlloc
Mem_BeginAlloc:
    jmp 0x81a74d0

global Mem_EndAlloc
Mem_EndAlloc:
    jmp 0x81a750a

