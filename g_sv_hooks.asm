SECTION .text

global G_RunFrame
G_RunFrame:
    jmp 0x80b61fc

global G_LocalizedStringIndex
G_LocalizedStringIndex:
    jmp 0x80cb218

global ClientCommand
ClientCommand:
    jmp 0x80b070c

global ClientConnect
ClientConnect:
    jmp 0x80a83d4

global ClientBegin
ClientBegin:
    jmp 0x80a7700

