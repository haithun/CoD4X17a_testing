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

global DB_SetInitializing
DB_SetInitializing:
    jmp 0x820337c

global DB_FileExists
DB_FileExists:
    jmp 0x8204424

global DB_ModFileExists
DB_ModFileExists:
    jmp 0x8204470

global DB_LoadXAssets
DB_LoadXAssets:
    jmp 0x8205e86

global DB_GetXAssetTypeSize
DB_GetXAssetTypeSize:
    jmp 0x81da6ce

global XAnimInit
XAnimInit:
    jmp 0x81b649c
