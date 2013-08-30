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

global Cvar_RegisterString
Cvar_RegisterString:
    jmp 0x81a2944

global Cvar_RegisterBool
Cvar_RegisterBool:
    jmp 0x81a2d94

global Cvar_RegisterInt
Cvar_RegisterInt:
    jmp 0x81a2cc6

global Cvar_RegisterEnum
Cvar_RegisterEnum:
    jmp 0x81a2860

global Cvar_RegisterFloat
Cvar_RegisterFloat:
    jmp 0x81a2e6c

global Cvar_SetInt
Cvar_SetInt:
    jmp 0x81a20c4

global Cvar_SetBool
Cvar_SetBool:
    jmp 0x81a1c6c

global Cvar_SetString
Cvar_SetString:
    jmp 0x81a14fa

global Cvar_SetFloat
Cvar_SetFloat:
    jmp 0x81a1fe0

global Cvar_Set_f
Cvar_Set_f:
    jmp 0x8127a80

global Cvar_SetS_f
Cvar_SetS_f:
    jmp 0x8127ce8

global Cvar_Toggle_f
Cvar_Toggle_f:
    jmp 0x8126f5c

global Cvar_TogglePrint_f
Cvar_TogglePrint_f:
    jmp 0x8126f66

global Cvar_SetA_f
Cvar_SetA_f:
    jmp 0x8127c7c

global Cvar_SetFromCvar_f
Cvar_SetFromCvar_f:
    jmp 0x812746a

global Cvar_SetFromLocalizedStr_f
Cvar_SetFromLocalizedStr_f:
    jmp 0x8127842

global Cvar_SetToTime_f
Cvar_SetToTime_f:
    jmp 0x81273aa

global Cvar_Reset_f
Cvar_Reset_f:
    jmp 0x8127356

global Cvar_List_f
Cvar_List_f:
    jmp 0x8127306

global Cvar_Dump_f
Cvar_Dump_f:
    jmp 0x81272d2

global Cvar_RegisterBool_f
Cvar_RegisterBool_f:
    jmp 0x8126fc6

global Cvar_RegisterInt_f
Cvar_RegisterInt_f:
    jmp 0x81276aa

global Cvar_RegisterFloat_f
Cvar_RegisterFloat_f:
    jmp 0x812751a

global Cvar_SetU_f
Cvar_SetU_f:
    jmp 0x8127d54

global g_cvar_valueforkey
g_cvar_valueforkey:
    jmp 0x819e90a

global Cvar_InfoString
Cvar_InfoString:
    jmp 0x81264f4

global Cvar_ForEach
Cvar_ForEach:
    jmp 0x819f328

global Cvar_DisplayableValue
Cvar_DisplayableValue:
    jmp 0x819e2ac

global Cvar_GetVariantString
Cvar_GetVariantString:
    jmp 0x819e8cc

global Cvar_FindMalleableVar
Cvar_FindMalleableVar:
    jmp 0x819e6d0