
#include <string.h>

#include "cmd.h"
#include "qcommon_io.h"
#include "qcommon_mem.h"
#include "qcommon.h"
#include "filesystem.h"


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


qboolean Cmd_SetPower(const char *cmd_name, int power)
{

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

int	Cmd_GetPower(const char* cmd_name)
{

    cmd_function_t *cmd;
    for(cmd = cmd_functions ; cmd ; cmd = cmd->next){
        if(!Q_stricmp(cmd_name,cmd->name)){

                if(!cmd->minPower) return 100;
                else return cmd->minPower;
        }
    }
    return -1; //Don't exist
}

void Cmd_ResetPower()
{
    cmd_function_t *cmd;
    //Init the permission table with default values
    for(cmd = cmd_functions ; cmd ; cmd = cmd->next) cmd->minPower = 100;
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


qboolean Cmd_InfoSetPower( const char *infostring )
{
        int power;
        char cmdname[40];

        power = atoi(Info_ValueForKey(infostring, "power"));
        Q_strncpyz(cmdname, Info_ValueForKey(infostring, "cmd"), sizeof(cmdname));

        if(!Cmd_SetPower(cmdname, power)){
            Com_DPrintf("Warning: Commandname %s is not known yet\n", cmdname);
            return qfalse;
        }
        return qtrue;

}


void Cmd_WritePowerConfig(char* buffer, int size)
{
    char infostring[1024];

    Q_strcat(buffer, size,"\n//Minimum power settings\n");
    cmd_function_t *cmd;
    for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ){
        *infostring = 0;
        Info_SetValueForKey(infostring, "type", "cmdMinPower");
        Info_SetValueForKey(infostring, "cmd", cmd->name);
        Info_SetValueForKey(infostring, "power", va("%i",cmd->minPower));
        Q_strcat(buffer, size, infostring);
        Q_strcat(buffer, size, "\\\n");
    }
}

/*
============
Cmd_ListPower_f
============
*/

static void Cmd_ListPower_f() {

	cmd_function_t  *cmd;
	int i, hidden, j, l;
	char            *match;

	if ( Cmd_Argc() > 1 ) {
		match = Cmd_Argv( 1 );
	} else {
		match = NULL;
	}

	i = 0;
	hidden = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( (match && !Com_Filter( match, cmd->name, qfalse ))) {
			continue;
		}
		if(cmd->minPower == 100){
			hidden++;
			continue;
		}
		Com_Printf ("%s", cmd->name );

		l = 24 - strlen(cmd->name);
		j = 0;

		do
		{
			Com_Printf (" ");
			j++;
		} while(j < l);

		Com_Printf( "%d\n", cmd->minPower );
		i++;
	}
	Com_Printf( "\n%i commands with specified power settings are shown\n", i );
	Com_Printf( "%i commands are hidden because the required power level for those commands is set to 100\n", hidden );
	Com_Printf( "Type cmdlist to get a complete list of all commands\n");
}



/*
============
Cmd_List_f
============
*/
static void Cmd_List_f( void ) {
	cmd_function_t  *cmd;
	int i;
	char            *match;

	if ( Cmd_Argc() > 1 ) {
		match = Cmd_Argv( 1 );
	} else {
		match = NULL;
	}

	i = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( (match && !Com_Filter( match, cmd->name, qfalse )) || SV_RemoteCmdGetInvokerPower() < cmd->minPower) {
			continue;
		}
		Com_Printf( "%s\n", cmd->name );
		i++;
	}
	Com_Printf( "%i commands\n", i );
}

/*
===============
Cmd_Exec_f
===============
*/
/*
void Cmd_Exec_f( void ) {
	char    *f;
	int len;
	int read;

	fileHandle_t file;
	char filename[MAX_QPATH];
	char buf[4096];

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "exec <filename> : execute a script file\n" );
		return;
	}

	Q_strncpyz( filename, Cmd_Argv( 1 ), sizeof( filename ) );
	COM_DefaultExtension( filename, sizeof( filename ), ".cfg" );
	FS_FOpenFileRead(filename, &file);
	//len = FS_ReadFile( filename, (void **)&f );
	if ( !file ) {
		Com_Printf( "couldn't exec %s\n",Cmd_Argv( 1 ) );
		return;
	}

	Com_Printf( "execing %s\n",Cmd_Argv( 1 ) );

	while(qtrue)
	{
		read = FS_ReadLine(buf, sizeof(buf), file);
		if(read == 0){
			FS_FCloseFile(file);
			return;
		}


		if(read == -1){
			Com_Printf("Can not read from nvconfig.dat\n");
			FS_FCloseFile(file);
			return;
		}
		if(!*buf || *buf == '\n'){
			continue;
		}
		Cbuf_Execute(buf, 0)
	}
}



void Cmd_Exec_f( void ) {
	char    *f;
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "exec <filename> : execute a script file\n" );
		return;
	}

	Q_strncpyz( filename, Cmd_Argv( 1 ), sizeof( filename ) );
	COM_DefaultExtension( filename, sizeof( filename ), ".cfg" );
	FS_ReadFile( filename, (void **)&f );
	if ( !f ) {
		Com_PrintError( "couldn't exec %s\n",Cmd_Argv( 1 ) );
		return;
	}
	Com_Printf( "execing %s\n",Cmd_Argv( 1 ) );

	Cbuf_ExecuteBuffer( 0,0, f );

	FS_FreeFile( f );
}
*/


void Cmd_Init( void ) {
	*(int*)0x88799a0 = -1;
	*(int*)0x887c300 = 0;
	*(int*)0x887c304 = 0;
	*(int*)0x8879a40 = -1;
	*(int*)0x887eb40 = 0;
	*(int*)0x887eb44 = 0;

	Cmd_AddCommand( "cmdlist",Cmd_List_f );
	Cmd_AddCommand ("cmdpowerlist", Cmd_ListPower_f);
	Cmd_AddCommand( "exec",Cmd_Exec_f );
	Cmd_AddCommand( "vstr",Cmd_Vstr_f );
	Cmd_AddCommand( "echo",Cmd_Echo_f );
	Cmd_AddCommand( "wait", Cmd_Wait_f );

}

