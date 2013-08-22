SECTION .text

global FS_CopyFile
FS_CopyFile:
    jmp 0x8189ec0

global FS_FOpenFileRead
FS_FOpenFileRead:
    jmp 0x818ba54

global FS_FOpenFileWrite
FS_FOpenFileWrite:
    jmp 0x818a428

global FS_FOpenFileAppend
FS_FOpenFileAppend:
    jmp 0x818a6cc

global unzOpen
unzOpen:
    jmp 0x81d3a09

global unzOpenCurrentFile
unzOpenCurrentFile:
    jmp 0x81d40fb

global unzSetOffset
unzSetOffset:
    jmp 0x81d35c5

global unzReadCurrentFile
unzReadCurrentFile:
    jmp 0x81d37db

global unzClose
unzClose:
    jmp 0x81d44aa

global unzCloseCurrentFile
unzCloseCurrentFile:
    jmp 0x81d3776

global FS_InitFilesystem
FS_InitFilesystem:
    jmp 0x818e980

global FS_Shutdown
FS_Shutdown:
    jmp 0x818733a

global FS_ShutdownIwdPureCheckReferences
FS_ShutdownIwdPureCheckReferences:
    jmp  0x81866b6

global FS_ShutdownServerIwdNames
FS_ShutdownServerIwdNames:
    jmp  0x8186cfe

global FS_ShutdownServerReferencedIwds
FS_ShutdownServerReferencedIwds:
    jmp  0x818789c

global FS_ShutdownServerReferencedFFs
FS_ShutdownServerReferencedFFs:
    jmp  0x8187850

global FS_LoadedIwdPureChecksums
FS_LoadedIwdPureChecksums:
    jmp  0x81283f2

global FS_GetMapBaseName
FS_GetMapBaseName:
    jmp  0x8127e0c