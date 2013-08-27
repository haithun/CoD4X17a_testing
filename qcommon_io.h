#ifndef __QCOMMON_STDIO_H__
#define __QCOMMON_STDIO_H__

#include "q_shared.h"

typedef enum{
    MSG_DEFAULT,
    MSG_NA,	//Not defined
    MSG_WARNING,
    MSG_ERROR,
    MSG_NORDPRINT
}msgtype_t;

// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *, qboolean) );
void Com_EndRedirect (void);
void Com_StopRedirect (void);

void QDECL Com_Printf( const char *fmt, ... );
void QDECL Com_PrintNoRedirect( const char *fmt, ... );
void QDECL Com_PrintError( const char *fmt, ... );
void QDECL Com_PrintWarning( const char *fmt, ... );
void QDECL Com_PrintWarningNoRedirect( const char *fmt, ... );
void QDECL Com_DPrintf( const char *fmt, ... );
void QDECL Com_DPrintfWrapper( int drop, const char *fmt, ...);
void QDECL Com_DPrintNoRedirect( const char *fmt, ... );
void QDECL Com_Error( int a, const char *error, ...);
void QDECL Com_PrintRedirect(char *msg, int msglen);
void Com_AddRedirect(void (*rd_dest)( const char *, int));
void __cdecl Com_ErrorCleanup(void);
void QDECL Com_PrintScriptRuntimeWarning( const char *fmt, ... );
__cdecl void Com_PrintMessage( int dumbIWvar, char *msg, msgtype_t type);

#endif
