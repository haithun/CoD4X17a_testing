#ifndef __QCOMMON_STDIO_H__
#define __QCOMMON_STDIO_H__

#include "q_shared.h"


// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

void QDECL Com_Printf( const char *fmt, ... );
void QDECL Com_PrintError( const char *fmt, ... );
void QDECL Com_PrintWarning( const char *fmt, ... );
void QDECL Com_DPrintf( const char *fmt, ... );
void QDECL Com_Error( int a, const char *error, ...);
void QDECL Com_PrintRedirect(char *msg, int msglen);

#endif