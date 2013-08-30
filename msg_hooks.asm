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


global MSG_ReadBits
MSG_ReadBits:
    jmp 0x8130730

global GetMinBitCount
GetMinBitCount:
    jmp 0x8130500

global MSG_WriteDeltaClient
MSG_WriteDeltaClient:
    jmp 0x813f49c

global MSG_WriteDeltaField
MSG_WriteDeltaField:
    jmp 0x813e22a

global MSG_WriteDeltaPlayerstate
MSG_WriteDeltaPlayerstate:
    jmp 0x813f5a6

global MSG_WriteEntityIndex
MSG_WriteEntityIndex:
    jmp 0x813de54

global MSG_WriteBit1
MSG_WriteBit1:
    jmp 0x81306dc

global MSG_WriteBits
MSG_WriteBits:
    jmp 0x813061c

global MSG_ReadDeltaUsercmdKey
MSG_ReadDeltaUsercmdKey:
    jmp 0x81392ae

global MSG_SetDefaultUserCmd
MSG_SetDefaultUserCmd:
    jmp 0x8130ad0

global MSG_WriteBitsCompress
MSG_WriteBitsCompress:
    jmp 0x8130f3c

global MSG_WriteReliableCommandToBuffer
MSG_WriteReliableCommandToBuffer:
    jmp 0x813e162