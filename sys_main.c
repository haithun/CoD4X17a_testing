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
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#include "q_shared.h"
#include "sys_main.h"
#include "q_platform.h"
#include "qcommon_io.h"
#include "qcommon.h"
#include "sys_cod4defs.h"
#include "sys_con_tty.h"
#include "filesystem.h"
#include "sys_cod4loader.h"
#include "sys_thread.h"
#include "punkbuster.h"
#include "server.h"

#include <sys/resource.h>
#include <libgen.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <wait.h>
#include <fpu_control.h>

#define MAX_QUED_EVENTS 256

#define MAX_CMD 1024
static char exit_cmdline[MAX_CMD] = "";

static char binaryPath[ MAX_OSPATH ] = { 0 };
static char installPath[ MAX_OSPATH ] = { 0 };

#ifndef MAXPRINTMSG
#define MAXPRINTMSG 1024
#endif


/*
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
	 0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
	 (which would affect the wrap period) */

unsigned int sys_timeBase;

unsigned int Sys_Milliseconds( void )
{
	struct timeval tp;

	gettimeofday( &tp, NULL );

	return ( tp.tv_sec - sys_timeBase ) * 1000 + tp.tv_usec / 1000;
}

unsigned long long Sys_MillisecondsLong( void )
{
	unsigned long long time;

	struct timeval tp;

	gettimeofday( &tp, NULL );

	time = (unsigned long long)tp.tv_sec - (unsigned long long)sys_timeBase;
	time = time * 1000 + tp.tv_usec / 1000;
	return time;
}


void Sys_TimerInit( void ) {
	struct timeval tp;

	gettimeofday( &tp, NULL );

	if ( !sys_timeBase )
	{
		sys_timeBase = tp.tv_sec;
	}
}


/*
================
Sys_Microseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
	 0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
	 (which would affect the wrap period) */
unsigned long long Sys_Microseconds( void ) {
	struct timeval tp;
	unsigned long long orgtime;
	unsigned long long time;

	gettimeofday( &tp, NULL );

	orgtime = tp.tv_sec - sys_timeBase;

	time = (orgtime << 19);
	time = time + (orgtime << 18);
	time = time + (orgtime << 17);
	time = time + (orgtime << 16);
	time = time + (orgtime << 14);
	time = time + (orgtime << 9);
	time = time + (orgtime << 6);
	time = time + (unsigned long long)tp.tv_usec;

	return time;
}


int Sys_Seconds( void ) {
	struct timeval tp;

	gettimeofday( &tp, NULL );

	return tp.tv_sec - sys_timeBase;
}

/*
=================
Sys_Exit

Single exit point (regular exit or in case of error)
=================
*/
static __attribute__ ((noreturn)) void Sys_Exit( int exitCode ) {

	CON_Shutdown();
	// we may be exiting to spawn another process
	if ( exit_cmdline[0] != '\0' ) {
		// possible race conditions?
		// buggy kernels / buggy GL driver, I don't know for sure
		// but it's safer to wait an eternity before and after the fork
		sleep( 1 );
		Sys_DoStartProcess( exit_cmdline );
		sleep( 1 );
	}

	// We can't do this
	// as long as GL DLL's keep installing with atexit...
	exit( exitCode );
}

/*
=================
Sys_Quit
=================
*/
void Sys_Quit( void )
{
	Sys_Exit( 0 );
}


/*
=================
Sys_Print
=================
*/
void Sys_Print( const char *msg )
{
//	CON_LogWrite( msg );
	CON_Print( msg );
}




/*
=================
Sys_SigHandler
=================
*/
void Sys_SigHandler( int signal )
{
	static qboolean signalcaught = qfalse;

	fprintf( stderr, "Received signal: %s, exiting...\n",
		strsignal(signal) );

	if( signalcaught )
	{
		fprintf( stderr, "DOUBLE SIGNAL FAULT: Received signal: %s, exiting...\n",
			strsignal(signal));
	}

	else
	{
		signalcaught = qtrue;
		Com_Printf("Server received signal: %s\nShutting down server...\n", strsignal(signal));
		SV_Shutdown(va("\nServer received signal: %s\nTerminating server...", strsignal(signal)) );

		Sys_EnterCriticalSection( 2 );

		FS_CloseLogFiles(); //close all open logfiles
		FS_Shutdown(qtrue);
	}

	if( signal == SIGTERM || signal == SIGINT )
		Sys_Exit( 1 );
	else
		Sys_Exit( 2 );
}


void Sys_TermProcess( )
{
    int status;
    wait(&status);
}



/*
=================
Sys_ConsoleInput

Handle new console input
=================
*/
char *Sys_ConsoleInput(void)
{
	return CON_Input( );
}


void Sys_PrintBinVersion( const char* name ) {

	char* sep = "==============================================================";
	fprintf( stdout, "\n\n%s\n", sep );

	fprintf( stdout, "%s %s %s build %i %s\n", GAME_STRING,Q3_VERSION,PLATFORM_STRING, BUILD_NUMBER, __DATE__);

	fprintf( stdout, " local install: %s\n", name );
	fprintf( stdout, "%s\n\n", sep );
}




void Sys_ParseArgs( int argc, char* argv[] ) {
	if ( argc == 2 ) {
		if ( ( !strcmp( argv[1], "--version" ) )
			 || ( !strcmp( argv[1], "-v" ) ) ) {
			Sys_PrintBinVersion( argv[0] );
			Sys_Exit( 0 );
		}
	}
}



/*
==============
Sys_PlatformInit

Unix specific initialisation
==============
*/
void Sys_PlatformInit( void )
{
	signal( SIGHUP, Sys_SigHandler );
	signal( SIGQUIT, Sys_SigHandler );
	signal( SIGTRAP, Sys_SigHandler );
	signal( SIGIOT, Sys_SigHandler );
	signal( SIGBUS, Sys_SigHandler );
//	signal( SIGCHLD, Sys_TermProcess );

}


/*
=============
Sys_Error

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
__cdecl DLL_PUBLIC void QDECL Sys_Error( const char *fmt, ... ) {

	FILE * fdout;
	char* fileout = "sys_error.txt";
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	char		buffer[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	sprintf(buffer, "Sys_Error: %s\n", msg);

	//Print the error to our console
	Sys_Print( buffer );

	//Try to write the error into a file
	fdout=fopen(fileout, "w");
	if(fdout){
		fwrite(buffer, strlen(buffer), 1 ,fdout);
		fclose(fdout);
	}

	Sys_Exit( 1 ); // bk010104 - use single exit point.
}


/*
=================
Sys_SetBinaryPath
=================
*/
void Sys_SetBinaryPath(const char *path)
{
	Q_strncpyz(binaryPath, path, sizeof(binaryPath));
}

/*
=================
Sys_BinaryPath
=================
*/
char *Sys_BinaryPath(void)
{
	return binaryPath;
}

/*
=================
Sys_SetDefaultInstallPath
=================
*/
void Sys_SetDefaultInstallPath(const char *path)
{
	Q_strncpyz(installPath, path, sizeof(installPath));
}

/*
=================
Sys_DefaultInstallPath
=================
*/
char *Sys_DefaultInstallPath(void)
{
	if (*installPath)
		return installPath;
	else
		return Sys_Cwd();
}

/*
=================
Sys_DefaultAppPath
=================
*/
char *Sys_DefaultAppPath(void)
{
	return Sys_BinaryPath();
}



#ifndef DEFAULT_BASEDIR
#	ifdef MACOS_X
#		define DEFAULT_BASEDIR Sys_StripAppBundle(Sys_BinaryPath())
#	else
#		define DEFAULT_BASEDIR Sys_BinaryPath()
#	endif
#endif




DLL_PUBLIC __cdecl int main(int argc, char* argv[]){

    int i;
    char commandLine[MAX_STRING_CHARS] = { 0 };

    // go back to real user for config loads
    seteuid(getuid());

    Sys_ParseArgs( argc, argv );

    Sys_SetBinaryPath( Sys_Dirname( argv[ 0 ] ) );

    Sys_SetDefaultInstallPath( DEFAULT_BASEDIR );

    Sys_LoadImage( );

    Sys_TimerInit( );

    Sys_PlatformInit( );

    Sys_ThreadInit();

    Sys_ThreadMain();

    Com_InitParse();

    commandLine[0] = 0;

    // Concatenate the command line for passing to Com_Init
    for( i = 1; i < argc; i++ )
    {
        const qboolean containsSpaces = strchr(argv[i], ' ') != NULL;
        if (containsSpaces)
            Q_strcat( commandLine, sizeof( commandLine ), "\"" );

        Q_strcat( commandLine, sizeof( commandLine ), argv[ i ] );

        if (containsSpaces)
            Q_strcat( commandLine, sizeof( commandLine ), "\"" );

        Q_strcat( commandLine, sizeof( commandLine ), " " );
    }


    CON_Init();

    Sys_InitCrashDumps();

    Com_Init( commandLine );


    signal( SIGILL, Sys_SigHandler );
    signal( SIGFPE, Sys_SigHandler );
    signal( SIGSEGV, Sys_SigHandler );
    signal( SIGTERM, Sys_SigHandler );
    signal( SIGINT, Sys_SigHandler );


    if(!PbServerInitialize()){
        Com_Printf("Unable to initialize PunkBuster.  PunkBuster is disabled.\n");
    }

    while ( 1 )
    {
        static int fpu_word = _FPU_DEFAULT;
        _FPU_SETCW( fpu_word );

        Com_Frame();
    }

}


