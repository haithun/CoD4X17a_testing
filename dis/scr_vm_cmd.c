scr_function_t *scr_functions = NULL;
scr_function_t *scr_methods = NULL;

/*
============
Scr_AddFunction
============
*/
qboolean Scr_AddFunction( const char *cmd_name, xcommand_t function, qboolean developer) {

	scr_function_t  *cmd;

	// fail if the command already exists
	for ( cmd = scr_functions ; cmd ; cmd = cmd->next ) {
		if ( !strcmp( cmd_name, cmd->name )) {
			// allow completion-only commands to be silently doubled
			if ( function != NULL ) {
				Com_PrintWarning("Scr_AddFunction: %s already defined\n", cmd_name);
			}
			return qfalse;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = Z_Malloc( sizeof( scr_function_t ) + strlen(cmd_name) + 1);
	strcpy((char*)(cmd +1), cmd_name);
	cmd->name = (char*)(cmd +1);
	cmd->function = function;
	cmd->developer = developer;
	cmd->next = scr_functions;
	scr_functions = cmd;
	return qtrue;
}

/*
============
Scr_RemoveFunction
============
*/
qboolean Scr_RemoveFunction( const char *cmd_name ) {
	scr_function_t  *cmd, **back;

	back = &scr_functions;
	while ( 1 ) {
		cmd = *back;
		if ( !cmd ) {
			// command wasn't active
			return qfalse;
		}
		if ( !strcmp( cmd_name, cmd->name ) ) {
			*back = cmd->next;
			Z_Free( cmd );
			return qtrue;
		}
		back = &cmd->next;
	}
	return qfalse;
}


/*
============
Scr_GetFunction
============
*/
DLL_PUBLIC __cdecl void* Scr_GetFunction( const char** v_functionName, qboolean* v_developer ) {

	scr_function_t  *cmd;

	for(cmd = scr_functions; cmd != NULL; cmd = cmd->next)
	{
		if(!Q_stricmp(*v_functionName, cmd->name))
		{
			*v_developer = cmd->developer;
			*v_functionName = cmd->name;
			return cmd->function;
		}
	}
	return NULL;
}

/*
============
Scr_AddMethod
============
*/
qboolean Scr_AddMethod( const char *cmd_name, xcommand_t function, qboolean developer) {

	scr_function_t  *cmd;

	// fail if the command already exists
	for ( cmd = scr_methods ; cmd ; cmd = cmd->next ) {
		if ( !strcmp( cmd_name, cmd->name )) {
			// allow completion-only commands to be silently doubled
			if ( function != NULL ) {
				Com_PrintWarning( "Scr_AddMethod: %s already defined\n", cmd_name );
			}
			return qfalse;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = Z_Malloc( sizeof( scr_function_t ) + strlen(cmd_name) + 1);
	strcpy((char*)(cmd +1), cmd_name);
	cmd->name = (char*)(cmd +1);
	cmd->function = function;
	cmd->developer = developer;
	cmd->next = scr_methods;
	scr_methods = cmd;
	return qtrue;
}

/*
============
Scr_RemoveMethod
============
*/
qboolean Scr_RemoveMethod( const char *cmd_name ) {
	scr_function_t  *cmd, **back;

	back = &scr_methods;
	while ( 1 ) {
		cmd = *back;
		if ( !cmd ) {
			// command wasn't active
			return qfalse;
		}
		if ( !strcmp( cmd_name, cmd->name ) ) {
			*back = cmd->next;
			Z_Free( cmd );
			return qtrue;
		}
		back = &cmd->next;
	}
	return qfalse;
}

/*
============
Scr_GetMethod
============
*/
DLL_PUBLIC __cdecl void* Scr_GetMethod( const char** v_functionName, qboolean* v_developer ) {

	scr_function_t  *cmd;

	for(cmd = scr_methods; cmd != NULL; cmd = cmd->next)
	{
		if(!Q_stricmp(*v_functionName, cmd->name))
		{
			*v_developer = cmd->developer;
			*v_functionName = cmd->name;
			return cmd->function;
		}
	}
	return NULL;
}