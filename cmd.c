/*
============
Cmd_AddCommand
============
*/
qboolean Cmd_AddCommand( const char *cmd_name, xcommand_t function ) {

	cmd_function_t  *cmd;

	// fail if the command already exists
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( !strcmp( cmd_name, cmd->name )) {
			// allow completion-only commands to be silently doubled
			if ( function != NULL ) {
				Com_PrintWarning( "Cmd_AddCommand: %s already defined\n", cmd_name );
			}
			return qfalse;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = Z_Malloc( sizeof( cmd_function_t ) + strlen(cmd_name) + 1);
	strcpy((char*)(cmd +1), cmd_name);
	cmd->name = (char*)(cmd +1);
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;
	return qtrue;
}

/*
============
Cmd_RemoveCommand
============
*/
qboolean Cmd_RemoveCommand( const char *cmd_name ) {
	cmd_function_t  *cmd, **back;

	back = &cmd_functions;
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
===============
Cmd_Echo_f

Just prints the rest of the line to the console
===============
*/
static void Cmd_Echo_f (void)
{
	char buf[MAX_STRING_CHARS];

	Com_Printf ("%s\n", Cmd_Args(buf, sizeof(buf)));
}



void Cmd_AddCommands(){

	static qboolean	initialized;

	if ( initialized ) {
		return;
	}
	initialized = qtrue;

	Cmd_AddCommand ("echo", Cmd_Echo_f);
}


qboolean Cmd_SetPower(const char *cmd_name, int power){

    cmd_function_t *cmd;
    if(!cmd_name) return qfalse;

    for(cmd = cmd_functions ; cmd ; cmd = cmd->next){
        if(!Q_stricmp(cmd_name, cmd->name)){
            if(cmd->minPower != power){
                cmd->minPower = power;
            }
            return qtrue;
        }
    }
    return qfalse;
}