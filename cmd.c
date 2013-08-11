#include <string.h>

#include "cmd.h"
#include "common_io.h"
#include "common_mem.h"



cmd_function_t **cmd_functions_addr = (cmd_function_t**)(0x887eb98);
#define cmd_functions *cmd_functions_addr


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



/*
============
SV_Cmd_Argc	Returns count of commandline arguments
============
*/
int	SV_Cmd_Argc( void ) {

	int	cmd_argc;

	__asm__ (
	"mov	0x8879a40,%%eax			\n\t"
	"mov	0x8879a84(,%%eax,4), %%eax	\n\t"
	:"=a" (cmd_argc));

	return cmd_argc;
}

/*
============
Cmd_Argc	Returns count of commandline arguments
============
*/
int	Cmd_Argc( void ) {

	int	cmd_argc;

	__asm__ (
	"mov	0x88799a0,%%eax			\n\t"
	"mov	0x88799e4(,%%eax,4), %%eax	\n\t"
	:"=a" (cmd_argc));
	return cmd_argc;
}


/*
============
SV_Cmd_Argv	Returns commandline argument by number
============
*/

char	*SV_Cmd_Argv( int arg ) {

	char* cmd_argv;

	__asm__ (
	"mov	0x8879a40,%%eax			\n\t"
	"mov    $0x822be98,%%edx		\n\t"
	"cmpl   %%ecx,0x8879a84(,%%eax,4)	\n\t"
	"jle	1f				\n\t"
	"mov    0x8879aa4(,%%eax,4),%%eax	\n\t"
	"lea	(%%eax,%%ecx,4),%%edx		\n\t"
	"mov    0x4(%%eax),%%edx		\n\t"
	"lea	(%%eax,%%ecx,4),%%edx		\n\t"
	"mov	(%%edx),%%edx			\n\t"
	"1:					\n\t"
	"					\n\t"
	:"=d" (cmd_argv)
	:"c" (arg)
	:"eax"					);
	return (cmd_argv);
}

/*
============
SV_Cmd_ArgvBuffer

The interpreted versions use this because
they can't have pointers returned to them
============
*/
void	SV_Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength ) {
	Q_strncpyz( buffer, SV_Cmd_Argv(arg), bufferLength );
}


/*
============
Cmd_Argv	Returns commandline argument by number
============
*/

char	*Cmd_Argv( int arg ) {

	char* cmd_argv;

	__asm__ (
	"mov	0x88799a0,%%eax			\n\t"
	"mov    $0x00,%%edx			\n\t"
	"cmpl   %%ecx,0x88799e4(,%%eax,4)	\n\t"
	"jle	1f				\n\t"
	"mov    0x8879a04(,%%eax,4),%%eax	\n\t" //?
	"lea	(%%eax,%%ecx,4),%%edx		\n\t"
	"mov	(%%edx),%%edx			\n\t"
	"1:					\n\t"
	"					\n\t"
	:"=d" (cmd_argv)
	:"c" (arg)
	:"eax"					);
	if(cmd_argv == NULL)
	    return "";

	else return (cmd_argv);
}

/*
============
Cmd_Args

Returns a single string containing argv(1) to argv(argc()-1)
============
*/

char	*Cmd_Args( char* buff, int bufsize ) {

	int		i;
	int		cmd_argc = Cmd_Argc();

	buff[0] = 0;
	for ( i = 1 ; i < cmd_argc ; i++ ) {
		Q_strcat( buff, bufsize, Cmd_Argv(i) );
		if ( i != cmd_argc-1 ) {
			Q_strcat( buff, bufsize, " " );
		}
	}

	return buff;
}


/*
============
Cmd_Argvs

Returns a single string containing argv(int arg) to argv(argc()-arg)
============
*/

char	*Cmd_Argsv( int arg, char* buff, int bufsize ) {

	int		i;
	int		cmd_argc = Cmd_Argc();
	buff[0] = 0;
	for ( i = arg ; i < cmd_argc ; i++ ) {
		Q_strcat( buff, bufsize, Cmd_Argv(i) );
		if ( i != cmd_argc-1 ) {
			Q_strcat( buff, bufsize, " " );
		}
	}

	return buff;
}


/*
===========================================
command line completion
===========================================
*/

/*
============
Cmd_CommandCompletion
============
*/
void	Cmd_CommandCompletion( void(*callback)(const char *s) ) {
	cmd_function_t	*cmd;
	
	for (cmd=cmd_functions ; cmd ; cmd=cmd->next) {
		callback( cmd->name );
	}
}

/*
============
Cmd_CompleteArgument
============
*/
void Cmd_CompleteArgument( const char *command, char *args, int argNum ) {
	cmd_function_t	*cmd;

	for( cmd = cmd_functions; cmd; cmd = cmd->next ) {
		if( !Q_stricmp( command, cmd->name ) && cmd->complete ) {
			cmd->complete( args, argNum );
		}
	}
}



/*
============
Cmd_SetCommandCompletionFunc
============
*/
void Cmd_SetCommandCompletionFunc( const char *command, completionFunc_t complete ) {
	cmd_function_t	*cmd;

	for( cmd = cmd_functions; cmd; cmd = cmd->next ) {
		if( !Q_stricmp( command, cmd->name ) ) {
			cmd->complete = complete;
		}
	}
}


