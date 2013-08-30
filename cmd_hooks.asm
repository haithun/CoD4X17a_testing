;===========================================================================
;    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

;    This file is part of CoD4X17a-Server source code.

;    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
;    it under the terms of the GNU Affero General Public License as
;    published by the Free Software Foundation, either version 3 of the
;    License, or (at your option) any later version.

;    CoD4X17a-Server source code is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU Affero General Public License for more details.

;    You should have received a copy of the GNU Affero General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>
;===========================================================================


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

