/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team
    Copyright (C) 1999-2005 Id Software, Inc.

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



#ifndef __SYS_MAIN_H__
#define __SYS_MAIN_H__

#include "q_shared.h"
#include <setjmp.h>


unsigned int Sys_Milliseconds( void );
unsigned long long Sys_MillisecondsLong( void );
unsigned long long Sys_MicrosecondsLong( void );

void Sys_TimerInit( void );
unsigned long long Sys_Microseconds( void );
int Sys_Seconds( void );
void Sys_Quit( void );
void Sys_Print( const char *msg );
void Sys_SigHandler( int signal );
void Sys_TermProcess(void);
char *Sys_ConsoleInput(void);
void Sys_AnsiColorPrint( const char *msg );
void Sys_PrintBinVersion( const char* name );
void Sys_ParseArgs( int argc, char* argv[] );
__cdecl void QDECL Sys_Error( const char *fmt, ... );
void Sys_SetBinaryPath(const char *path);
char *Sys_BinaryPath(void);
void Sys_SetDefaultInstallPath(const char *path);
char *Sys_DefaultInstallPath(void);
char *Sys_DefaultAppPath(void);
//sys_unix.c
qboolean Sys_RandomBytes( byte *string, int len );
void Sys_DoStartProcess( char *cmdline );
const char *Sys_Dirname( char *path );
char *Sys_Cwd( void );
void Sys_InitCrashDumps();
qboolean Sys_MemoryProtectWrite(void* startoffset, int len);
qboolean Sys_MemoryProtectExec(void* startoffset, int len);
qboolean Sys_MemoryProtectReadonly(void* startoffset, int len);
const char *Sys_DefaultHomePath(void);
const char *Sys_TempPath( void );
void __cdecl Sys_Init(void);

#endif











