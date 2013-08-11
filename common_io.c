#include "q_shared.h"
//#include "sys_main.h"
//#include "sys_thread.h"
//#include "filesystem.h"
#include "common_io.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>


#ifndef Sys_EnterCriticalSection
void Sys_EnterCriticalSection(int section){}
#endif
#ifndef Sys_EnterCriticalSection
void Sys_LeaveCriticalSection(int section){}
#endif
#ifndef Sys_Print
#define Sys_Print(x) fputs(x, stdout);
#endif
#ifndef FS_PrintLogfile
void FS_PrintLogfile( char* msg ){}
#endif
#ifndef Com_IsDeveloper
qboolean Com_IsDeveloper( void ){ return qtrue; }
#endif


#define MAXPRINTMSG 1024

//============================================================================
static char	*rd_buffer;
static int	rd_buffersize;
static void	(*rd_flush)( char *buffer, qboolean );


typedef enum{
    MSG_DEFAULT,
    MSG_NA,	//Not defined
    MSG_WARNING,
    MSG_ERROR,
    MSG_NORDPRINT
}msgtype_t;


void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *, qboolean) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

void Com_EndRedirect (void)
{
	if ( rd_flush ) {
		rd_flush(rd_buffer, qtrue);
	}

	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
}

void Com_StopRedirect (void)
{
	rd_flush = NULL;
}

DLL_PUBLIC __cdecl void Com_PrintMessage( int dumbIWvar, char *msg, msgtype_t type) {


	if(dumbIWvar == 6) return;

	int msglen = strlen(msg);

	Sys_EnterCriticalSection(5);

	if ( type != MSG_NORDPRINT) {
		Com_PrintRedirect(msg, msglen);

		if ( rd_buffer ) {
			if(!rd_flush){
				Sys_LeaveCriticalSection(5);
				return;
			}
			if ((msglen + strlen(rd_buffer)) > (rd_buffersize - 1)) {
				rd_flush(rd_buffer, qfalse);
				*rd_buffer = 0;
			}
			Q_strcat(rd_buffer, rd_buffersize, msg);
			// TTimo nooo .. that would defeat the purpose
			//rd_flush(rd_buffer);
			//*rd_buffer = 0;
			Sys_LeaveCriticalSection(5);
			return;
		}
	}
	// echo to dedicated console and early console
	Sys_Print( msg );

	// logfile
	FS_PrintLogfile( msg );
}

/*
=============
Com_Printf

Both client and server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_DEFAULT);
}


/*
=============
Com_PrintfNoRedirect

This will not print to rcon

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintNoRedirect( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_NORDPRINT);
}


/*
=============
Com_PrintWarning

Server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintWarning( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	memcpy(msg,"^3Warning: ", sizeof(msg));

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[11], (sizeof(msg)-12), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_WARNING);
}


/*
=============
Com_PrintWarningNoRedirect

Server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintWarningNoRedirect( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	memcpy(msg,"^3Warning: ", sizeof(msg));

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[11], (sizeof(msg)-12), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_NORDPRINT);
}


/*
=============
Com_PrintError

Server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintError( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	memcpy(msg,"^1Error: ", sizeof(msg));

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[9], (sizeof(msg)-10), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_ERROR);
}

/*
================
Com_DPrintf

A Com_Printf that only shows up if the "developer" cvar is set
================
*/
void QDECL Com_DPrintf( const char *fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];
		
	if ( !Com_IsDeveloper() ) {
		return;			// don't confuse non-developers with techie stuff...
	}
	
	msg[0] = '^';
	msg[1] = '2';

	va_start (argptr,fmt);	
	Q_vsnprintf (&msg[2], (sizeof(msg)-3), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_DEFAULT);
}


void QDECL Com_DPrintfWrapper( int drop, const char *fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];
		
	if ( !Com_IsDeveloper() ) {
		return;			// don't confuse non-developers with techie stuff...
	}
	
	msg[0] = '^';
	msg[1] = '2';

	va_start (argptr,fmt);	
	Q_vsnprintf (&msg[2], (sizeof(msg)-3), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_DEFAULT);
}

/*
================
Com_DPrintNoRedirect

A Com_Printf that only shows up if the "developer" cvar is set
This will not print to rcon
================
*/
void QDECL Com_DPrintNoRedirect( const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !Com_IsDeveloper() ) {
		return;			// don't confuse non-developers with techie stuff...
	}
	
	msg[0] = '^';
	msg[1] = '2';

	va_start (argptr,fmt);	
	Q_vsnprintf (&msg[2], (sizeof(msg)-3), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_NORDPRINT);
}


#define MAX_REDIRECTDESTINATIONS 4

static void (*rd_destinations[MAX_REDIRECTDESTINATIONS])( char *buffer, int len );


void Com_PrintRedirect(char* msg, int msglen)
{

    int i;

    for(i = 0; i < MAX_REDIRECTDESTINATIONS; i++)
    {
        if(rd_destinations[i] == NULL)
            return;

        rd_destinations[i](msg, msglen);

    }

}


/*
    To Add:
    PbCapatureConsoleOutput(msg, MAXPRINTMSG);
    HL2Rcon_SourceRconSendConsole( msg, msglen);
    Com_PrintUDP( msg, msglen );
*/


void Com_AddRedirect(void (*rd_dest)( char *, int))
{
    int i;

    for(i = 0; i < MAX_REDIRECTDESTINATIONS; i++)
    {
        if(rd_destinations[i] == rd_dest)
        {
            Com_Error(ERR_FATAL, "Com_AddRedirect: Attempt to add an already defined redirect function twice.");
            return;
        }

        if(rd_destinations[i] == NULL)
        {
            rd_destinations[i] = rd_dest;
            return;
        }
    }
    Com_Error(ERR_FATAL, "Com_AddRedirect: Out of redirect handles. Increase MAX_REDIRECTDESTINATIONS to add more redirect destinations");
}