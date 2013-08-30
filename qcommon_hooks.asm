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

global Com_Error
Com_Error:
    jmp 0x812257c

global Com_InitParse
Com_InitParse:
    jmp 0x81a7a78

global Com_Restart
Com_Restart:
    jmp 0x8123586

global Com_InitHunkMemory
Com_InitHunkMemory:
    jmp 0x8197174

global Hunk_InitDebugMemory
Hunk_InitDebugMemory:
    jmp 0x819752a

global Com_WriteConfig_f
Com_WriteConfig_f:
    jmp 0x8122e82

global Com_WriteDefaults_f
Com_WriteDefaults_f:
    jmp 0x8123bae

global Com_ErrorCleanup
Com_ErrorCleanup:
    jmp 0x8123c86

global Hunk_ClearTempMemory
Hunk_ClearTempMemory:
    jmp 0x81968a8

global Hunk_ClearTempMemoryHigh
Hunk_ClearTempMemoryHigh:
    jmp 0x81968b8

global Com_Close
Com_Close:
    jmp 0x8121b82

global Hunk_AllocateTempMemory
Hunk_AllocateTempMemory:
    jmp 0x8196fea

global Hunk_FreeTempMemory
Hunk_FreeTempMemory:
    jmp 0x81969d4

global Z_Malloc
Z_Malloc:
    jmp 0x8196d6e

global Com_Filter
Com_Filter:
    jmp 0x819837e

global Mem_Init
Mem_Init:
    jmp 0x81a75e6

global Mem_BeginAlloc
Mem_BeginAlloc:
    jmp 0x81a74d0

global Mem_EndAlloc
Mem_EndAlloc:
    jmp 0x81a750a

global TempMalloc
TempMalloc:
    jmp 0x8151dce

global Com_DvarDump
Com_DvarDump:
    jmp 0x8126764
