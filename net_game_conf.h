/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// server.h

//#include "../game/q_shared.h"
//#include "../qcommon/qcommon.h"
//#include "../game/g_public.h"
//#include "../game/bg_public.h"

//=============================================================================

#ifndef __NET_GAMECONF_H__
#define __NET_GAMECONF_H__

#ifndef AUTHORIZE_SERVER_NAME
#define	AUTHORIZE_SERVER_NAME	"cod4master.activision.com"
#endif

#ifndef PORT_AUTHORIZE
#define	PORT_AUTHORIZE		20800
#endif

#define	PORT_SERVER		28960

#define	PROTOCOL_VERSION	6

#define MAX_MSGLEN 0x20000

#endif