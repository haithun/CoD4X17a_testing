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



#ifndef __SYS_THREAD_H__
#define __SYS_THREAD_H__

void __cdecl Sys_EnterCriticalSection(int section);
void __cdecl Sys_LeaveCriticalSection(int section);
void __cdecl Sys_ThreadInit( void );
void __cdecl Sys_ThreadMain( void );
qboolean __cdecl Sys_IsMainThread( void );
void Com_InitThreadData(void);
const void* __cdecl Sys_GetValue(int key);
void __cdecl Sys_SetValue(int key, const void* value);

#endif