SECTION .text

global Cmd_Init
Cmd_Init:
    jmp 0x8111730

global Cmd_TokenizeString
Cmd_TokenizeString:
    jmp 0x811142c

global SV_Cmd_TokenizeString
SV_Cmd_TokenizeString:
    jmp 0x811139c

global Cmd_EndTokenizeString
Cmd_EndTokenizeString:
    jmp 0x8110d54

global SV_Cmd_EndTokenizeString
SV_Cmd_EndTokenizeString:
    jmp 0x8110d8c

global Cmd_ExecuteSingleCommand
Cmd_ExecuteSingleCommand:
    jmp 0x8111bea

global Cbuf_Init 
Cbuf_Init:
    jmp 0x81110bc

global Cbuf_Execute
Cbuf_Execute:
    jmp 0x8111f3c

global Cbuf_ExecuteBuffer
Cbuf_ExecuteBuffer:
    jmp 0x81120ae

global Cbuf_AddText
Cbuf_AddText:
    jmp 0x8110ff8

