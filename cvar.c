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

#include "cvar.h"
#include "cmd.h"
#include "cmd_completion.h"
// nothing outside the Cvar_*() functions should modify these fields!

#ifndef __CMD_COMPLETION_H__
void Cvar_CompleteCvarName(){}
#endif


void Cvar_VariableStringBuffer(const char* cvarname, char* buff, size_t size){
    Q_strncpyz(buff, Cvar_GetVariantString(cvarname), size);
}

/*
============
Cvar_Init

Reads in all archived cvars
============
*/
void Cvar_Init (void)
{
	Cmd_AddCommand ("toggle", Cvar_Toggle_f);
	Cmd_SetCommandCompletionFunc( "toggle", Cvar_CompleteCvarName );
	Cmd_AddCommand ("togglep", Cvar_TogglePrint_f);
	Cmd_SetCommandCompletionFunc( "togglep", Cvar_CompleteCvarName );
	Cmd_AddCommand ("set", Cvar_Set_f);
	Cmd_SetCommandCompletionFunc( "set", Cvar_CompleteCvarName );
	Cmd_AddCommand ("sets", Cvar_SetS_f);
	Cmd_SetCommandCompletionFunc( "sets", Cvar_CompleteCvarName );
	Cmd_AddCommand ("seta", Cvar_SetA_f);
	Cmd_SetCommandCompletionFunc( "seta", Cvar_CompleteCvarName );
	Cmd_AddCommand ("setfromdvar", Cvar_SetFromCvar_f);
	Cmd_SetCommandCompletionFunc( "setfromdvar", Cvar_CompleteCvarName );
	Cmd_AddCommand ("setfromlocString", Cvar_SetFromLocalizedStr_f);
	Cmd_SetCommandCompletionFunc( "setfromlocString", Cvar_CompleteCvarName );
	Cmd_AddCommand ("setdvartotime", Cvar_SetToTime_f);
	Cmd_SetCommandCompletionFunc( "setdvartotime", Cvar_CompleteCvarName );
	Cmd_AddCommand ("reset", Cvar_Reset_f);
	Cmd_SetCommandCompletionFunc( "reset", Cvar_CompleteCvarName );
	Cmd_AddCommand ("setu", Cvar_SetU_f);
	Cmd_SetCommandCompletionFunc( "setu", Cvar_CompleteCvarName );
	Cmd_AddCommand ("dvarlist", Cvar_List_f);
	Cmd_AddCommand ("dvardump", Cvar_Dump_f);
	Cmd_AddCommand ("dvar_bool", Cvar_RegisterBool_f);
	Cmd_AddCommand ("dvar_int", Cvar_RegisterInt_f);
	Cmd_AddCommand ("dvar_float", Cvar_RegisterFloat_f);
	cvar_t	**tmp = (cvar_t**)(0x14078480);
	*tmp = Cvar_RegisterBool("sv_cheats", qfalse, CVAR_INIT, "Enable cheating on server");
}


/*
============
Cvar_VariableValue
============
*/
float Cvar_VariableValue( const char *var_name ) {
	cvar_t	*var;
	
	var = Cvar_FindMalleableVar (var_name);
	if (!var || var->type != CVAR_FLOAT)
		return 0;
	return var->value;
}


/*
============
Cvar_VariableIntegerValue
============
*/
int Cvar_VariableIntegerValue( const char *var_name ) {
	cvar_t	*var;
	
	var = Cvar_FindMalleableVar (var_name);

	if (!var || var->type != CVAR_INT)
		return 0;
	return var->integer;
}


/*
============
Cvar_VariableString
============
*/
const char* Cvar_VariableString( const char *var_name ) {
	cvar_t *var;
	
	var = Cvar_FindMalleableVar (var_name);
	if (!var || var->type != CVAR_STRING)
		return "";
	return var->string;
}