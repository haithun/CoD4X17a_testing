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

global Scr_InitVariables
Scr_InitVariables:
      jmp 0x815288a

global Scr_Init
Scr_Init:
      jmp 0x815d8e2

global Scr_Settings
Scr_Settings:
      jmp 0x815cf90

global Scr_AddEntity
Scr_AddEntity:
      jmp 0x80c7770

global Scr_Cleanup
Scr_Cleanup:
      jmp 0x815cf84

global GScr_Shutdown
GScr_Shutdown:
      jmp 0x80bf610

global Scr_AllocArray
Scr_AllocArray:
      jmp 0x8153cca

global Scr_GetNumParam
Scr_GetNumParam:
      jmp 0x815d01e

global Scr_GetInt
Scr_GetInt:
      jmp 0x8160fee

global Scr_GetFloat
Scr_GetFloat:
      jmp 0x816094c

global Scr_GetString
Scr_GetString:
      jmp 0x8160932

global Scr_GetEntity
Scr_GetEntity:
      jmp 0x80c76ec

global Scr_GetConstString
Scr_GetConstString:
      jmp 0x816074c

global Scr_GetType
Scr_GetType:
      jmp 0x815f7c8

global Scr_GetVector
Scr_GetVector:
      jmp 0x815ffe6

global Scr_Error
Scr_Error:
      jmp 0x815e9f4

global Scr_SetLoading
Scr_SetLoading:
      jmp 0x815cfba

global Scr_ParamError
Scr_ParamError:
      jmp 0x815f20e

global Scr_ObjectError
Scr_ObjectError:
      jmp 0x815f134

global Scr_AddInt
Scr_AddInt:
      jmp 0x815f01a

global Scr_AddFloat
Scr_AddFloat:
      jmp 0x815ef9a

global Scr_AddBool
Scr_AddBool:
      jmp 0x815eac6

global Scr_AddString
Scr_AddString:
      jmp 0x815ec68

global Scr_AddUndefined
Scr_AddUndefined:
      jmp 0x815eea2

global Scr_AddVector
Scr_AddVector:
      jmp 0x815ee12

global Scr_AddArray
Scr_AddArray:
      jmp 0x815d5c0

global Scr_MakeArray
Scr_MakeArray:
      jmp 0x815ed8a

global Scr_Notify
Scr_Notify:
      jmp 0x80c7604

global Scr_NotifyNum
Scr_NotifyNum:
      jmp 0x815e762

;Not working :(
global Scr_PrintPrevCodePos
Scr_PrintPrevCodePos:
      jmp 0x814ef6e

global Scr_GetFunctionHandle
Scr_GetFunctionHandle:
      jmp 0x814c1b4

global Scr_ExecEntThread
Scr_ExecEntThread:
      jmp 0x80c765c

global Scr_ExecThread
Scr_ExecThread:
      jmp 0x8165032

global Scr_FreeThread
Scr_FreeThread:
      jmp 0x815d062

global Scr_CreateCanonicalFilename
Scr_CreateCanonicalFilename:
      jmp 0x81516ee

global FindVariable
FindVariable:
      jmp 0x81542d4

global FindObject
FindObject:
      jmp 0x8152294

global GetNewVariable
GetNewVariable:
      jmp 0x81545ce

global ScriptParse
ScriptParse:
      jmp 0x816b5da

global GetObjectA
GetObjectA:
      jmp 0x8154046

global GetVariable
GetVariable:
      jmp 0x815540a

global ScriptCompile
ScriptCompile:
      jmp 0x81491d8

global Scr_AddSourceBuffer
Scr_AddSourceBuffer:
      jmp 0x814fbac

global Scr_InitAllocNode
Scr_InitAllocNode:
      jmp 0x814fea6

global Scr_BeginLoadScripts
Scr_BeginLoadScripts:
      jmp 0x814c266

global Scr_SetClassMap
Scr_SetClassMap:
      jmp 0x8153a3a

global Scr_AddFields
Scr_AddFields:
      jmp 0x81535ba

global Scr_SetGenericField
Scr_SetGenericField:
      jmp 0x80c7eae

global Scr_GetGenericField
Scr_GetGenericField:
      jmp 0x80c7d36

global GScr_AddFieldsForHudElems
GScr_AddFieldsForHudElems:
      jmp 0x808db80

global GScr_AddFieldsForRadiant
GScr_AddFieldsForRadiant:
      jmp 0x80c77ec

global Scr_AddHudElem
Scr_AddHudElem:
      jmp 0x80c7746

global Scr_FreeHudElem
Scr_FreeHudElem:
      jmp 0x80c778e

global Scr_EndLoadScripts
Scr_EndLoadScripts:
      jmp 0x814bcee

global Scr_ConstructMessageString
Scr_ConstructMessageString:
      jmp 0x80c02aa
