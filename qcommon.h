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



#ifndef __QCOMMON_H__
#define __QCOMMON_H__

#include <time.h>
#include "cvar.h"

void __cdecl Com_InitParse( void );
void __cdecl Com_InitCvars(void);
void __cdecl Com_Restart(void);
void __cdecl Com_WriteConfig_f(void);
void __cdecl Com_WriteDefaults_f(void);
void __cdecl Com_DvarDump(int, int);
void __cdecl Com_Close(void);
void QDECL Com_Error( int level, const char *error, ...);
int __cdecl Com_Filter( char* filter, char *name, int casesensitive);

void Com_Frame(void);
void Com_Init(char* cmdline);
void Com_UpdateRealtime();
time_t Com_GetRealtime();
int QDECL Com_AddTimedEvent( int delay, void *function, unsigned int argcount, ...);

void Com_RandomBytes( byte *string, int len );
int Com_HashKey( char *string, int maxlen );
void Com_Quit_f( void );
unsigned int Com_GetFrameTime(void);
unsigned long long Com_GetUsecFrameTime(void);

extern unsigned long long com_frameTime;
extern unsigned long long com_uFrameTime;
extern cvar_t* com_dedicated;
extern cvar_t* com_timescale;

int Com_IsDeveloper();


#endif