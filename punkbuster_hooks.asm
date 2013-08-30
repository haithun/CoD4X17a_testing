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

global PbServerInitialize
PbServerInitialize:
    jmp 0x810ecde

global PbServerProcessEvents
PbServerProcessEvents:
    jmp 0x810ef08

global PbAuthClient
PbAuthClient:
    jmp 0x810e47a

global PbPassConnectString
PbPassConnectString:
    jmp 0x810e47a

global PbSvAddEvent
PbSvAddEvent:
    jmp 0x810ea20

global PbCapatureConsoleOutput
PbCapatureConsoleOutput:
    jmp 0x810e66a

global PbServerForceProcess
PbServerForceProcess:
    jmp 0x810ee36