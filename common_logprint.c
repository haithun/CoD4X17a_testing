#include "q_shared.h"
#include "qcommon.h"
#include "filesystem.h"
#include "sys_thread.h"

#include <stdarg.h>
#include <time.h>
#include <string.h>

#ifndef MAXPRINTMSG
#define MAXPRINTMSG 1024
#endif

static fileHandle_t logfile;
static fileHandle_t adminlogfile;
static fileHandle_t debuglogfile;
static fileHandle_t enterleavelogfile;


void QDECL SV_EnterLeaveLog( const char *fmt, ... ) {

	Sys_EnterCriticalSection(5);

	va_list		argptr;
	char		msg[MAXPRINTMSG];
	char		inputmsg[MAXPRINTMSG];
	struct tm 	*newtime;
	char*		ltime;
	time_t		realtime;

        // logfile
	if ( com_logfile && com_logfile->integer ) {
        // TTimo: only open the qconsole.log if the filesystem is in an initialized state
        // also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)

	    va_start (argptr,fmt);
	    Q_vsnprintf (inputmsg, sizeof(inputmsg), fmt, argptr);
	    va_end (argptr);

	    Com_UpdateRealtime();
	    realtime = Com_GetRealtime();
	    newtime = localtime( &realtime );
	    ltime = asctime( newtime );
	    ltime[strlen(ltime)-1] = 0;

	    if ( !enterleavelogfile && FS_Initialized()) {

			enterleavelogfile = FS_FOpenFileAppend( "enterleave.log" );
			// force it to not buffer so we get valid
			if ( enterleavelogfile ){
				FS_ForceFlush(enterleavelogfile);
				FS_Write(va("\nLogfile opened on %s\n\n", ltime), strlen(va("\nLogfile opened on %s\n\n", ltime)), enterleavelogfile);
			}
	    }

	    if ( enterleavelogfile && FS_Initialized()) {
		Com_sprintf(msg, sizeof(msg), "%s: %s\n", ltime, inputmsg);
		FS_Write(msg, strlen(msg), enterleavelogfile);
	    }

	}
	Sys_LeaveCriticalSection(5);
}


void QDECL Com_PrintAdministrativeLog( const char *msg ) {

	Sys_EnterCriticalSection(5);

	struct tm 	*newtime;
	char*		ltime;
	time_t		realtime;

        // logfile
	if ( com_logfile && com_logfile->integer ) {
        // TTimo: only open the qconsole.log if the filesystem is in an initialized state
        //   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)


	    if ( !adminlogfile && FS_Initialized()) {

			Com_UpdateRealtime();
			realtime = Com_GetRealtime();
			newtime = localtime( &realtime );
			ltime = asctime( newtime );
			ltime[strlen(ltime)-1] = 0;


			adminlogfile = FS_FOpenFileAppend( "adminactions.log" );
			// force it to not buffer so we get valid
			if ( adminlogfile ){
				FS_ForceFlush(adminlogfile);
				FS_Write(va("\nLogfile opened on %s\n\n", ltime), strlen(va("\nLogfile opened on %s\n\n", ltime)), adminlogfile);
			}
	    }

	    if ( adminlogfile && FS_Initialized())
	    {
		FS_Write(msg, strlen(msg), adminlogfile);
	    }

	}
	Sys_LeaveCriticalSection(5);
}

void Com_PrintLogfile( const char *msg )
{

	if ( com_logfile && com_logfile->integer ) {
        // TTimo: only open the qconsole.log if the filesystem is in an initialized state
        // also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
	    if ( !logfile && FS_Initialized()) {
			struct tm *newtime;
			time_t aclock;

			time( &aclock );
			newtime = localtime( &aclock );

			logfile = FS_FOpenFileWrite( "qconsole.log" );

			if ( com_logfile->integer > 1 && logfile ) {
				// force it to not buffer so we get valid
				// data even if we are crashing
				FS_ForceFlush(logfile);
			}
			if ( logfile ) FS_Write(va("\nLogfile opened on %s\n", asctime( newtime )), strlen(va("\nLogfile opened on %s\n", asctime( newtime ))), logfile);
	    }
	    if ( logfile && FS_Initialized()) 
	    {
	    	FS_Write(msg, strlen(msg), logfile);
	    }
	}
	Sys_LeaveCriticalSection(5);
}


/*
This function should close all opened non Zip files
*/
void Com_CloseLogFiles()
{
	if(adminlogfile)
		FS_FCloseFile( adminlogfile );
	if(logfile)
		FS_FCloseFile( logfile );
	if(debuglogfile)
		FS_FCloseFile( debuglogfile );
	if(enterleavelogfile)
		FS_FCloseFile( enterleavelogfile );
}
