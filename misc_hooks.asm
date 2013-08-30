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

global SL_Init
SL_Init:
    jmp 0x8150928

global Swap_Init
Swap_Init:
    jmp 0x81aa7b6

global CSS_InitConstantConfigStrings
CSS_InitConstantConfigStrings:
    jmp 0x8185a72

global Con_InitChannels
Con_InitChannels:
    jmp 0x82096be

global SEH_UpdateLanguageInfo
SEH_UpdateLanguageInfo:
    jmp 0x8180432

global SetAnimCheck
SetAnimCheck:
    jmp 0x81423f0

global BG_IsWeaponValid
BG_IsWeaponValid:
    jmp 0x805f4fe

global SEH_StringEd_GetString
SEH_StringEd_GetString:
    jmp 0x817fbe0

global DObjInit
DObjInit:
    jmp 0x81acb00

global SL_RemoveRefToString
SL_RemoveRefToString:
    jmp 0x8150e24