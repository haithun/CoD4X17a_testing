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

#ifndef __CVAR_H__
#define __CVAR_H__

#include "q_math.h"
#include "q_shared.h"

// nothing outside the Cvar_*() functions should modify these fields!

typedef enum{
    CVAR_BOOL,
    CVAR_FLOAT,
    CVAR_VEC2,
    CVAR_VEC3,
    CVAR_VEC4,
    CVAR_INT,
    CVAR_ENUM,
    CVAR_STRING,
    CVAR_COLOR
}cvarType_t;



typedef struct{
	char *name;
	char *description;
	short int flags;
	byte type;
	byte modified;
	union{
		float floatval;
		float value;
		int integer;
		char* string;
		byte boolean;
		vec3_t vec3;
		vec4_t vec4;
	};
	union{
		float latchedfloatval;
		int latchedinteger;
		char* latchedstring;
		byte latchedboolean;
		vec3_t vec3_latched;
		vec4_t vec4_latched;
	};
	union{
		float resetfloatval;
		int resetinteger;
		char* resetstring;
		byte resetboolean;
		vec3_t vec3_reset;
		vec4_t vec4_reset;
	};
	int min;
	int max;
	int unknown3;
	int unknown4;
} cvar_t;


//Defines Cvarrelated functions inside executable file
 cvar_t* __cdecl Cvar_RegisterString(const char *var_name, const char *var_value, int flags, const char *var_description);
 cvar_t* __cdecl Cvar_RegisterBool(const char *var_name, qboolean var_value, int flags, const char *var_description);
 cvar_t* __cdecl Cvar_RegisterInt(const char *var_name, int var_value, int min_value, int max_value, int flags, const char *var_description);
 cvar_t* __cdecl Cvar_RegisterEnum(const char *var_name, char** valnames, int defaultval, int flags, const char *var_description);
 cvar_t* __cdecl Cvar_RegisterFloat(const char *var_name, float var_value, float min_value, float max_value, int flags, const char *var_description);
 void __cdecl Cvar_SetInt(cvar_t const* var, int val);
 void __cdecl Cvar_SetBool(cvar_t const* var, qboolean val);
 void __cdecl Cvar_SetString(cvar_t const* var, char const* string);
 void __cdecl Cvar_SetFloat(cvar_t const* var, float val);
 void __cdecl Cvar_Set_f(void);
 void __cdecl Cvar_SetS_f(void);
 void __cdecl Cvar_Toggle_f(void);
 void __cdecl Cvar_TogglePrint_f(void);
 void __cdecl Cvar_SetA_f(void);
 void __cdecl Cvar_SetFromCvar_f(void);
 void __cdecl Cvar_SetFromLocalizedStr_f(void);
 void __cdecl Cvar_SetToTime_f(void);
 void __cdecl Cvar_Reset_f(void);
 void __cdecl Cvar_List_f(void);
 void __cdecl Cvar_Dump_f(void);
 void __cdecl Cvar_RegisterBool_f(void);
 void __cdecl Cvar_RegisterInt_f(void);
 void __cdecl Cvar_RegisterFloat_f(void);
 void __cdecl Cvar_SetU_f(void);
 int __cdecl g_cvar_valueforkey(char* key);
 char* __cdecl Cvar_InfoString(int unk, int bit);
 void __cdecl Cvar_ForEach(void (*callback)(cvar_t const*, void* passedhere), void* passback);
 char* __cdecl Cvar_DisplayableValue(cvar_t const*);
 char* __cdecl Cvar_GetVariantString(const char* name);
 cvar_t* __regparm1 Cvar_FindMalleableVar(const char* name);

//defines Cvarflags
#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_LATCH			32	// will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_TEMP			256	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT		128	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued








//This defines Cvars directly related to executable file
#define getcvaradr(adr) ((cvar_t*)(*(int*)(adr)))

#define sv_mapname getcvaradr(0x13ed8974)
#define sv_maxclients getcvaradr(0x13ed8960)
#define com_sv_running getcvaradr(0x88a61a8)
#define sv_maxRate getcvaradr(0x13ed897c)
#define g_gametype getcvaradr(0x13ed89bc)
#define sv_pure getcvaradr(0x13ed89d0)
#define sv_fps getcvaradr(0x13ed8950)
#define com_developer getcvaradr(0x88a6184)
#define useFastfiles getcvaradr(0x88a6170)
#define com_logfile getcvaradr(0x88a61b0)
#define com_developer_script getcvaradr(0x88a6188)
#define sv_packet_info getcvaradr(0x13ed89f0)
#define g_gravity getcvaradr(0x84bcff4)
#define jump_height getcvaradr(0x8278060)
#define jump_stepSize getcvaradr(0x8278070)
#define jump_slowdownEnable getcvaradr(0x8278068)

#define g_TeamName_Allies getcvaradr(0x84bd090)
#define g_TeamName_Axis getcvaradr(0x84bd094)
//#define com_playerProfile getcvaradr(0x88e7394)

#define g_allowVote getcvaradr(0x84bd05c)
#define sv_serverid getcvaradr(0x13ed8978)
#define sv_floodProtect getcvaradr(0x13ed89e4)

#endif




















