#include <string.h>
#include "q_shared.h"
#include "cmd_completion.h"
#include "qcommon_io.h"

/*
===========================================
command line completion
===========================================
*/

#define MAX_TOKEN_CHARS 1024

/*
==================
Field_Clear
==================
*/
void Field_Clear( field_t *edit ) {
	memset(edit->buffer, 0, MAX_EDIT_LINE);
	edit->cursor = 0;
	edit->scroll = 0;
}

static const char	*completionString;
static char		shortestMatch[MAX_TOKEN_CHARS];
static int		matchCount;
// field we are working on, passed to Field_AutoComplete(&g_consoleCommand for instance)
static field_t		*completionField;



/*
===============
FindMatches

===============
*/
static void FindMatches( const char *s ) {
	int		i;

	if ( Q_stricmpn( s, completionString, strlen( completionString ) ) ) {
		return;
	}
	matchCount++;
	if ( matchCount == 1 ) {
		Q_strncpyz( shortestMatch, s, sizeof( shortestMatch ) );
		return;
	}

	// cut shortestMatch to the amount common with s
	for ( i = 0 ; shortestMatch[i] ; i++ ) {
		if ( i >= strlen( s ) ) {
			shortestMatch[i] = 0;
			break;
		}

		if ( tolower(shortestMatch[i]) != tolower(s[i]) ) {
			shortestMatch[i] = 0;
		}
	}
}

/*
===============
PrintMatches

===============
*/
static void PrintMatches( const char *s ) {
	if ( !Q_stricmpn( s, shortestMatch, strlen( shortestMatch ) ) ) {
		Com_Printf( "    %s\n", s );
	}
}



/*
===============
Field_FindFirstSeparator
===============
*/
static char *Field_FindFirstSeparator( char *s )
{
	int i;

	for( i = 0; i < strlen( s ); i++ )
	{
		if( s[ i ] == ';' )
			return &s[ i ];
	}

	return NULL;
}

/*
===============
Field_Complete
===============
*/
static qboolean Field_Complete( void )
{
	int completionOffset;

	if( matchCount == 0 )
		return qtrue;

	completionOffset = strlen( completionField->buffer ) - strlen( completionString );

	Q_strncpyz( &completionField->buffer[ completionOffset ], shortestMatch,
		sizeof( completionField->buffer ) - completionOffset );

	completionField->cursor = strlen( completionField->buffer );

	if( matchCount == 1 )
	{
		Q_strcat( completionField->buffer, sizeof( completionField->buffer ), " " );
		completionField->cursor++;
		return qtrue;
	}

	Com_Printf( "]%s\n", completionField->buffer );

	return qfalse;
}

/*
===============
Field_CompleteFilename
===============
*//*
void Field_CompleteFilename( const char *dir,
		const char *ext, qboolean stripExt, qboolean allowNonPureFilesOnDisk )
{
	matchCount = 0;
	shortestMatch[ 0 ] = 0;

	FS_FilenameCompletion( dir, ext, stripExt, FindMatches, allowNonPureFilesOnDisk );

	if( !Field_Complete( ) )
		FS_FilenameCompletion( dir, ext, stripExt, PrintMatches, allowNonPureFilesOnDisk );
}
*/

/*
===============
Field_CompleteCommand
===============
*/

void Field_CompleteCommand( char *cmd, qboolean doCommands, qboolean doCvars )
{
	int		completionArgument = 0;

	// Skip leading whitespace and quotes
	cmd = Com_SkipCharset( cmd, " \"" );

	Cmd_TokenizeString( cmd );
	completionArgument = Cmd_Argc( );




	// If there is trailing whitespace on the cmd
	if( *( cmd + strlen( cmd ) - 1 ) == ' ' )
	{
		completionString = "";
		completionArgument++;
	}
	else
		completionString = Cmd_Argv( completionArgument - 1 );




	if( completionArgument > 1 )
	{
		const char *baseCmd = Cmd_Argv( 0 );
		char *p;

		if( ( p = Field_FindFirstSeparator( cmd ) ) )
			Field_CompleteCommand( p + 1, qtrue, qtrue ); // Compound command
		else
			Cmd_CompleteArgument( baseCmd, cmd, completionArgument );

	}else{



		if( completionString[0] == '\\' || completionString[0] == '/' )
			completionString++;

		matchCount = 0;
		shortestMatch[ 0 ] = 0;
		
		if( strlen( completionString ) == 0 ){
			Cmd_EndTokenizeString( );
			return;
		}

		if( doCommands )
			Cmd_CommandCompletion( FindMatches );

		if( doCvars )
			Cvar_ForEach(Cvar_CommandCompletionFind, NULL);

		if( !Field_Complete( ) )
		{
			// run through again, printing matches
			if( doCommands )
				Cmd_CommandCompletion( PrintMatches );

			if( doCvars )
				Cvar_ForEach(Cvar_CommandCompletionPrint, NULL);
		}
	}
	Cmd_EndTokenizeString( );
}

/*
===============
Field_AutoComplete

Perform Tab expansion
===============
*/
void Field_AutoComplete( field_t *field )
{
	completionField = field;

	Field_CompleteCommand( completionField->buffer, qtrue, qtrue );
}


/*
============
Cvar_CommandCompletion
============
*/
void	Cvar_CommandCompletionPrint( cvar_t const *cvar, void* none) {
		PrintMatches( cvar->name );
}

void	Cvar_CommandCompletionFind( cvar_t const *cvar, void* none) {
		FindMatches( cvar->name );
}


/*
==================
Cvar_CompleteCvarName
==================
*/
void Cvar_CompleteCvarName( char *args, int argNum )
{
	if( argNum == 2 )
	{
		// Skip "<cmd> "
		char *p = Com_SkipTokens( args, 1, " " );

		if( p > args )
			Field_CompleteCommand( p, qfalse, qtrue );
	}
}