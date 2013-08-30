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

global SV_StringUsage_f
SV_StringUsage_f:
    jmp 0x814cc7e

global SV_ScriptUsage_f
SV_ScriptUsage_f:
    jmp 0x81557c4

global SV_BeginClientSnapshot
SV_BeginClientSnapshot:
    jmp 0x817a32e

global SV_EndClientSnapshot
SV_EndClientSnapshot:
    jmp 0x817b488

global SV_ClientThink
SV_ClientThink:
    jmp 0x816df7a

global SV_SpawnServer
SV_SpawnServer:
    jmp 0x8174a68

global SV_SetGametype
SV_SetGametype:
    jmp 0x817c72c

global SV_InitCvars
SV_InitCvars:
    jmp 0x819e66c

global SV_RestartGameProgs
SV_RestartGameProgs:
    jmp 0x817c68a

global SV_ResetSekeletonCache
SV_ResetSekeletonCache:
    jmp 0x817c602

global SV_PreFrame
SV_PreFrame:
    jmp 0x8177534

global SV_SendClientMessages
SV_SendClientMessages:
    jmp 0x817b71e

global SV_SetServerStaticHeader
SV_SetServerStaticHeader:
    jmp 0x8178fd4

global SV_ShutdownGameProgs
SV_ShutdownGameProgs:
    jmp 0x817c818

global SV_FreeClients
SV_FreeClients:
    jmp 0x81720c6

global SV_GameSendServerCommand
SV_GameSendServerCommand:
    jmp 0x817ce42

global SV_SetConfigstring
SV_SetConfigstring:
    jmp 0x8173fda

global SV_FreeClient
SV_FreeClient:
    jmp 0x81708bc

global SV_FreeClientScriptId
SV_FreeClientScriptId:
    jmp 0x8175c5e

global SV_LinkEntity
SV_LinkEntity:
    jmp 0x817eb20

global SV_UnlinkEntity
SV_UnlinkEntity:
    jmp 0x817d5e0

global SV_AddServerCommand_old
SV_AddServerCommand_old:
    jmp 0x817664c
