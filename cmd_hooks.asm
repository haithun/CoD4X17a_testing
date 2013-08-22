SECTION .text

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

global Cmd_Wait_f
Cmd_Wait_f:
    jmp 0x81111dc

global Cmd_Vstr_f
Cmd_Vstr_f:
    jmp 0x8111290

global Cmd_Exec_f
Cmd_Exec_f:
    jmp 0x81121a2

