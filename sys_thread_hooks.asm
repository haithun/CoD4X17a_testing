SECTION .text

global Sys_IsMainThread
Sys_IsMainThread:
    jmp 0x8140efe

global Sys_EnterCriticalSection
Sys_EnterCriticalSection:
    jmp 0x81d6be4

global Sys_LeaveCriticalSection
Sys_LeaveCriticalSection:
    jmp 0x81d6bc8

global Sys_ThreadInit
Sys_ThreadInit:
    jmp 0x81d6c00

global Sys_ThreadMain
Sys_ThreadMain:
    jmp 0x8140f5c