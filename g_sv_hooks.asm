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

global StopFollowing
StopFollowing:
    jmp 0x80af24c

global G_SayTo
G_SayTo:
    jmp 0x80ae6f4
