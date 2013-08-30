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



//This are dummy functions for the single thread server :D

#include "q_shared.h"
#include "sys_thread.h"

void __cdecl Sys_EnterCriticalSection(int section)
{

}

void __cdecl Sys_LeaveCriticalSection(int section)
{

}

void __cdecl Sys_ThreadInit( void )
{

}

void __cdecl Sys_ThreadMain( void )
{
    Com_InitThreadData();
}

qboolean __cdecl Sys_IsMainThread( void )
{
	return qtrue;
}


#define MAX_KEYS 3

const void* sys_valuestoreage[MAX_KEYS];

const void* __cdecl Sys_GetValue(int key)
{
    return sys_valuestoreage[key +1];
}

void __cdecl Sys_SetValue(int key, const void* value)
{
    sys_valuestoreage[key +1] = value;
}



void Com_InitThreadData()
{

    Sys_SetValue(1, (const void*)0x1408f6a0);
    Sys_SetValue(2, (const void*)0x140df6c0);
    Sys_SetValue(3, (const void*)0x14087620);
}