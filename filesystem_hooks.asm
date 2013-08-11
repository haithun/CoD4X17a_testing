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

