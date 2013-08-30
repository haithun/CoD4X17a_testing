/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#ifndef __SYS_COD4DEFS_H__
#define __SYS_COD4DEFS_H__


#define COD4_DLL "cod4_lnxded.so"
#define COD4_BINNAME "cod4_lnxded"

#define TEXT_SECTION_OFFSET 0x2c20
#define TEXT_SECTION_LENGTH 0x1bf1a4
#define RODATA_SECTION_OFFSET 0x1c1e00
#define RODATA_SECTION_LENGTH 0x36898
#define DATA_SECTION_OFFSET 0x222580
#define DATA_SECTION_OFFSET_FIX 0x1000
#define DATA_SECTION_LENGTH 0x9454
#define IMAGE_BASE 0x8048000




#define Q3_VERSION "1.7a"
#define GAME_STRING "CoD4 X"

#include "version.h"
#ifndef BUILD_NUMBER
#define BUILD_NUMBER -1
#endif

#define MAX_CLIENTS 64
#define MAX_CONFIGSTRINGS 2442

#endif