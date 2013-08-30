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
