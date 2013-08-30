/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#include "q_shared.h"
#include "qcommon_io.h"
#include "qcommon_parsecmdline.h"
#include "qcommon_logprint.h"
#include "sys_cod4defs.h"
#include "cvar.h"
#include "filesystem.h"
#include "qcommon_mem.h"
#include "q_platform.h"
#include "sys_main.h"
#include "sys_thread.h"
#include "qcommon.h"
#include "cmd.h"
#include "sys_net.h"
#include "xassets.h"
#include "plugin_handler.h"
#include "misc.h"
#include "scr_vm.h"
#include "netchan.h"
#include "server.h"
#include "nvconfig.h"
#include "hl2rcon.h"

#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>

#include "common_adrdefs.h"


unsigned long long com_uFrameTime = 0;
unsigned long long com_frameTime = 0;

cvar_t* com_version;
cvar_t* com_shortversion;
cvar_t* com_dedicated;
cvar_t* com_timescale;
cvar_t* com_fixedtime;
cvar_t* com_maxFrameTime;
cvar_t* com_animCheck;
cvar_t* com_developer;

/*
========================================================================

EVENT LOOP

========================================================================
*/

typedef union{
    float f;
    char c;
    int i;
    qboolean b;
    byte by;
    void* p;
}universalArg_t;


typedef enum {
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,    // evTime is still valid
	SE_CONSOLE, // evPtr is a char*
	SE_PACKET   // evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct {
	int evTime;
	sysEventType_t evType;
	int evValue, evValue2;
	int evPtrLength;                // bytes of data pointed to by evPtr, for journaling
	void            *evPtr;         // this must be manually freed if not NULL
} sysEvent_t;


#define MAX_TIMEDEVENTARGS 8

typedef struct{
    universalArg_t arg;
    unsigned int size;
}timedEventArg_t;

typedef timedEventArg_t timedEventArgs_t[MAX_TIMEDEVENTARGS];


#define MAX_QUEUED_EVENTS  256
#define MAX_TIMED_EVENTS  1024
#define MASK_QUEUED_EVENTS ( MAX_QUEUED_EVENTS - 1 )
#define MASK_TIMED_EVENTS ( MAX_TIMED_EVENTS - 1 )

typedef struct{
	int evTime, evTriggerTime;
	timedEventArgs_t evArguments;
	void (*evFunction)();
}timedSysEvent_t;


static sysEvent_t  eventQueue[ MAX_QUEUED_EVENTS ];
static timedSysEvent_t  timedEventBuffer[ MAX_QUEUED_EVENTS ];
static int         eventHead = 0;
static int         eventTail = 0;
static int         timedEventHead = 0;


void EventTimerTest(int time, int triggerTime, int value, char* s){

	Com_Printf("^5Event exectuted: %i %i %i %i %s\n", time, triggerTime, Sys_Milliseconds(), value, s);

}


/*
================
Com_SetTimedEventCachelist

================
*/
void Com_MakeTimedEventArgCached(unsigned int index, unsigned int arg, unsigned int size){

	if(index >= MAX_TIMED_EVENTS)
		Com_Error(ERR_FATAL, "Com_MakeTimedEventArgCached: Bad index: %d", index);

	if(arg >= MAX_TIMEDEVENTARGS)
		Com_Error(ERR_FATAL, "Com_MakeTimedEventArgCached: Bad function argument number. Allowed range is 0 - %d arguments", MAX_TIMEDEVENTARGS);

	timedSysEvent_t  *ev = &timedEventBuffer[index];
	void *ptr = Z_Malloc(size);
	Com_Memcpy(ptr, ev->evArguments[arg].arg.p, size);
	ev->evArguments[arg].size = size;
	ev->evArguments[arg].arg.p = ptr;
}


/*
================
Com_AddTimedEvent

================
*/
int QDECL Com_AddTimedEvent( int delay, void *function, unsigned int argcount, ...)
{
	timedSysEvent_t  *ev;
	int index;
	int i;
	int time;
	int triggerTime;

	if ( timedEventHead >= MAX_TIMED_EVENTS )
	{
		Com_PrintWarning("Com_AddTimedEvent: overflow - Lost one event\n");
		// we are discarding an event, but don't leak memory
		return -1;
	}

	index = timedEventHead;

	time = Sys_Milliseconds();

	triggerTime = delay + time;

	while(qtrue)
	{
		if(index > 0){

			ev = &timedEventBuffer[index -1];

			if(ev->evTriggerTime < triggerTime)
			{
				timedEventBuffer[index] = *ev;
				index--;
				continue;
			}
		}
		break;
	}

	if(argcount > MAX_TIMEDEVENTARGS)
	{
		Com_Error(ERR_FATAL, "Com_AddTimedEvent: Bad number of function arguments. Allowed range is 0 - %d arguments", MAX_TIMEDEVENTARGS);
		return -1;
	}

	ev = &timedEventBuffer[index];

	va_list		argptr;
	va_start(argptr, argcount);

	for(i = 0; i < MAX_TIMEDEVENTARGS; i++)
	{
		if(i < argcount)
			ev->evArguments[i].arg = va_arg(argptr, universalArg_t);

		ev->evArguments[i].size = 0;
	}

	va_end(argptr);

	ev->evTime = time;
	ev->evTriggerTime = triggerTime;
	ev->evFunction = function;
	timedEventHead++;
	return index;
}



void Com_InitEventQueue()
{
    // bk000306 - clear eventqueue
    memset( eventQueue, 0, MAX_QUEUED_EVENTS * sizeof( sysEvent_t ) );
}

/*
================
Com_QueueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Com_QueueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t  *ev;

	ev = &eventQueue[ eventHead & MASK_QUEUED_EVENTS ];

	if ( eventHead - eventTail >= MAX_QUEUED_EVENTS )
	{
		Com_PrintWarning("Com_QueueEvent: overflow\n");
		// we are discarding an event, but don't leak memory
		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	if ( time == 0 )
	{
		time = Sys_Milliseconds();
	}

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
================
Com_GetTimedEvent

================
*/
timedSysEvent_t* Com_GetTimedEvent( int time )
{
	timedSysEvent_t  *ev;

	if(timedEventHead > 0)
	{
		ev = &timedEventBuffer[timedEventHead - 1];
		if(ev->evTriggerTime <= time)
		{
			timedEventHead--; //We have removed one event
			return ev;
		}
	}
	return NULL;
}


/*
================
Com_GetSystemEvent

================
*/
sysEvent_t* Com_GetSystemEvent( void )
{
	char        *s;
	// return if we have data

	if ( eventHead > eventTail )
	{
		eventTail++;
		return &eventQueue[ ( eventTail - 1 ) & MASK_QUEUED_EVENTS ];
	}

	// check for console commands
	s = Sys_ConsoleInput();
	if ( s )
	{
		char  *b;
		int   len;

		len = strlen( s ) + 1;
		b = Z_Malloc( len );
		strcpy( b, s );
		Com_QueueEvent( 0, SE_CONSOLE, 0, 0, len, b );
	}

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		return &eventQueue[ ( eventTail - 1 ) & MASK_QUEUED_EVENTS ];
	}

	// create an empty event to return
	return NULL;
}


/*
=================
Com_EventLoop

Returns last event time
=================
*/
void Com_EventLoop( void ) {
	sysEvent_t	*ev;

	while ( 1 ) {
		ev = Com_GetSystemEvent();

		// if no more events are available
		if ( !ev ) {
			break;
		}
			switch(ev->evType)
			{
				case SE_CONSOLE:
					Cbuf_AddText( 0,(char *)ev->evPtr );
					Cbuf_AddText(0,"\n");
				break;
				default:
					Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev->evType );
				break;
			}
			// free any block data
			if ( ev->evPtr ) {
				Z_Free( ev->evPtr );
			}
	}
}


/*
=================
Com_TimedEventLoop
=================
*/
void Com_TimedEventLoop( void ) {
	timedSysEvent_t	*evt;
	int time = Sys_Milliseconds();
	int i;

	while( qtrue ) {
		evt = Com_GetTimedEvent(time);

		// if no more events are available
		if ( !evt ) {
			break;
		}
		//Execute the passed eventhandler
		if(evt->evFunction)
			evt->evFunction(evt->evArguments[0].arg, evt->evArguments[1].arg, evt->evArguments[2].arg, evt->evArguments[3].arg,
			evt->evArguments[4].arg, evt->evArguments[5].arg, evt->evArguments[6].arg, evt->evArguments[7].arg);

		for(i = 0; i < MAX_TIMEDEVENTARGS; i++)
		{
			if(evt->evArguments[i].size > 0){
				Z_Free(evt->evArguments[i].arg.p);
			}
		}
	}
}


int Com_IsDeveloper()
{
    if(com_developer && com_developer->integer)
        return com_developer->integer;

    return 0;

}

/*
=============
Com_Error_f

Just throw a fatal error to
test error shutdown procedures
=============
*/
static void Com_Error_f (void) {
	if ( Cmd_Argc() > 1 ) {
		Com_Error( ERR_DROP, "Testing drop error" );
	} else {
		Com_Error( ERR_FATAL, "Testing fatal error" );
	}
}


/*
=============
Com_Freeze_f

Just freeze in place for a given number of seconds to test
error recovery
=============
*/
static void Com_Freeze_f (void) {
	float	s;
	int		start, now;

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "freeze <seconds>\n" );
		return;
	}
	s = atof( Cmd_Argv(1) );

	start = Sys_Milliseconds();

	while ( 1 ) {
		now = Sys_Milliseconds();
		if ( ( now - start ) * 0.001 > s ) {
			break;
		}
	}
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
static void Com_Crash_f( void ) {
	* ( int * ) 0 = 0x12345678;
}


/*
==================
Com_RandomBytes

fills string array with len radom bytes, peferably from the OS randomizer
==================
*/
void Com_RandomBytes( byte *string, int len )
{
	int i;

	if( Sys_RandomBytes( string, len ) )
		return;

	Com_Printf( "Com_RandomBytes: using weak randomization\n" );
	for( i = 0; i < len; i++ )
		string[i] = (unsigned char)( rand() % 255 );
}


/*
============
Com_HashKey
============
*/
int Com_HashKey( char *string, int maxlen ) {
	int register hash, i;

	hash = 0;
	for ( i = 0; i < maxlen && string[i] != '\0'; i++ ) {
		hash += string[i] * ( 119 + i );
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	return hash;
}


/*
=============
Com_Quit_f

Both client and server can use this, and it will
do the apropriate things.
=============
*/
void Com_Quit_f( void ) {
	// don't try to shutdown if we are in a recursive error
	Com_Printf("quitting...\n");
	Scr_Cleanup();
	Sys_EnterCriticalSection( 2 );
	GScr_Shutdown();

	if ( !com_errorEntered ) {
		// Some VMs might execute "quit" command directly,
		// which would trigger an unload of active VM error.
		// Sys_Quit will kill this process anyways, so
		// a corrupt call stack makes no difference
		Hunk_ClearTempMemory();
		Hunk_ClearTempMemoryHigh();
		SV_Shutdown("EXE_SERVERQUIT");

		Com_Close();

		Com_CloseLogFiles( );

		FS_Shutdown(qtrue);
		FS_ShutdownIwdPureCheckReferences();
		FS_ShutdownServerIwdNames();
		FS_ShutdownServerReferencedIwds();
		FS_ShutdownServerReferencedFFs();
		NET_Shutdown();
	}
	Sys_Quit ();
}


void Com_InitCvars( void ){
    static char* dedicatedEnum[] = {"listen server", "dedicated LAN server", "dedicated internet server", NULL};
    static char* logfileEnum[] = {"disabled", "async file write", "sync file write", NULL};

    char* s;

    com_dedicated = Cvar_RegisterEnum("dedicated", dedicatedEnum, 2, 0x40, "True if this is a dedicated server");
    com_timescale = Cvar_RegisterFloat("timescale", 1.0, 0.0, 1000.0, CVAR_CHEAT | CVAR_SYSTEMINFO, "Scale time of each frame");
    com_fixedtime = Cvar_RegisterInt("fixedtime", 0, 0, 1000, 0x80, "Use a fixed time rate for each frame");
    com_maxFrameTime = Cvar_RegisterInt("com_maxFrameTime", 100, 50, 1000, 0, "Time slows down if a frame takes longer than this many milliseconds");
    com_animCheck = Cvar_RegisterBool("com_animCheck", qfalse, 0, "Check anim tree");
    s = va("%s %s %s %s", GAME_STRING, Q3_VERSION, PLATFORM_STRING, __DATE__ );

    com_version = Cvar_RegisterString ("version", s, CVAR_ROM | CVAR_SERVERINFO , "Game version");
    com_shortversion = Cvar_RegisterString ("shortversion", Q3_VERSION, CVAR_ROM | CVAR_SERVERINFO , "Short game version");


    Cvar_RegisterString ("build", va("%i", BUILD_NUMBER), CVAR_ROM | CVAR_SERVERINFO , "");

    cvar_t** tmp;
    tmp = (cvar_t**)(0x88a6170);
    *tmp = Cvar_RegisterBool ("useFastFils", qtrue, 16, "Enables loading data from fast files");
    //MasterServer
    //AuthServer
    //MasterServerPort
    //AuthServerPort
    com_developer = Cvar_RegisterInt("developer", 0, 0, 2, 0, "Enable development options");
    tmp = (cvar_t**)(0x88a6184);
    *tmp = com_developer;

    tmp = (cvar_t**)(0x88a6188);
    *tmp = Cvar_RegisterBool ("developer_script", qfalse, 16, "Enable developer script comments");
    tmp = (cvar_t**)(0x88a61b0);
    *tmp = Cvar_RegisterEnum("logfile", logfileEnum, 0, 0, "Write to logfile");
    tmp = (cvar_t**)(0x88a61a8);
    *tmp = Cvar_RegisterBool("sv_running", qfalse, 64, "Server is running");


}




/*
=================
Com_Init

The games main initialization
=================
*/



void Com_Init(char* commandLine){


    static char creator[16];
    char creatorname[37];
    int msec = 0;
    int	qport;

    jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(2);

    if(setjmp(*abortframe)){
        Sys_Error(va("Error during Initialization:\n%s\n", com_lastError));
    }
    Com_Printf("%s %s %s build %i %s\n", GAME_STRING,Q3_VERSION,PLATFORM_STRING, BUILD_NUMBER, __DATE__);

    XAssets_PatchLimits();  //Patch several asset-limits to higher values

    SL_Init();

    Swap_Init();

    Cbuf_Init();

    Cmd_Init();

    Com_InitEventQueue();

    Com_ParseCommandLine(commandLine);

    Com_StartupVariable(NULL);

    Com_InitCvars();

    Cvar_Init();

    CSS_InitConstantConfigStrings();

    if(useFastfiles->integer){

        Mem_Init();

        DB_SetInitializing( qtrue );

        Com_Printf("begin $init\n");

        msec = Sys_Milliseconds();

        Mem_BeginAlloc("$init", qtrue);
    }

    FS_InitFilesystem();

    Con_InitChannels();

    Cbuf_AddText(0, "exec default_mp.cfg\n");
    Cbuf_Execute(0,0); // Always execute after exec to prevent text buffer overflowing

    Com_StartupVariable(NULL);

    if(!useFastfiles->integer) SEH_UpdateLanguageInfo();

    Com_InitHunkMemory();

    Hunk_InitDebugMemory();

    creator[0] = '_';
    creator[1] = 'C';
    creator[2] = 'o';
    creator[3] = 'D';
    creator[4] = '4';
    creator[5] = ' ';
    creator[6] = 'X';

    creator[7] = ' ';
    creator[8] = 'C';
    creator[9] = 'r';
    creator[10] = 'e';
    creator[11] = 'a';
    creator[12] = 't';
    creator[13] = 'o';
    creator[14] = 'r';
    creator[15] = '\0';

    creatorname[0] = 'N';
    creatorname[1] = 'i';
    creatorname[2] = 'n';
    creatorname[3] = 'j';
    creatorname[4] = 'a';
    creatorname[5] = 'm';
    creatorname[6] = 'a';
    creatorname[7] = 'n';
    creatorname[8] = ',';
    creatorname[9] = ' ';
    creatorname[10] = 'T';
    creatorname[11] = 'h';
    creatorname[12] = 'e';
    creatorname[13] = 'K';
    creatorname[14] = 'e';
    creatorname[15] = 'l';
    creatorname[16] = 'm';
    creatorname[17] = ' ';
    creatorname[18] = '@';
    creatorname[19] = ' ';
    creatorname[20] = 'h';
    creatorname[21] = 't';
    creatorname[22] = 't';
    creatorname[23] = 'p';
    creatorname[24] = ':';
    creatorname[25] = '/';
    creatorname[26] = '/';
    creatorname[27] = 'i';
    creatorname[28] = 'c';
    creatorname[29] = 'e';
    creatorname[30] = 'o';
    creatorname[31] = 'p';
    creatorname[32] = 's';
    creatorname[33] = '.';
    creatorname[34] = 'i';
    creatorname[35] = 'n';
    creatorname[36] = '\0';

    Cvar_RegisterString (creator, creatorname, CVAR_ROM | CVAR_SERVERINFO , "");

    cvar_modifiedFlags &= ~CVAR_ARCHIVE;

    com_codeTimeScale = 0x3f800000;

    if (com_developer && com_developer->integer)
    {
        Cmd_AddCommand ("error", Com_Error_f);
        Cmd_AddCommand ("crash", Com_Crash_f);
        Cmd_AddCommand ("freeze", Com_Freeze_f);
    }
    Cmd_AddCommand ("quit", Com_Quit_f);

//    Com_AddLoggingCommands();
//    HL2Rcon_AddSourceAdminCommands();

    Sys_Init();

    Com_UpdateRealtime();

    Com_RandomBytes( (byte*)&qport, sizeof(int) );
    Netchan_Init( qport );

    Scr_InitVariables();

    Scr_Init(); //VM_Init

    Scr_Settings(com_logfile->integer || com_developer->integer ,com_developer_script->integer, com_developer->integer);

    XAnimInit();

    DObjInit();

    PHandler_Init();

    NET_Init();

    SV_Init();

    com_frameTime = Sys_Milliseconds();

    Mem_EndAlloc("$init", qtrue);
    DB_SetInitializing( qfalse );
    Com_Printf("end $init %d ms\n", Sys_Milliseconds() - msec);

    if(useFastfiles->integer)
        R_Init();

    Com_DvarDump(6,0);

    NV_LoadConfig();

    Com_Printf("--- Common Initialization Complete ---\n");

    Cbuf_Execute( 0, 0 );

    Com_AddStartupCommands( );


    abortframe = (jmp_buf*)Sys_GetValue(2);

    if(setjmp(*abortframe)){
        Sys_Error(va("Error during Initialization:\n%s\n", com_lastError));
    }
    if(com_errorEntered) Com_ErrorCleanup();


    HL2Rcon_Init();

    AddRedirectLocations();

}




/*
================
Com_ModifyUsec
================
*/

unsigned int Com_ModifyUsec( unsigned int usec ) {
	int		clampTime;

	//
	// modify time for debugging values
	//
	if ( com_fixedtime->integer ) {
		usec = com_fixedtime->integer*1000;
	} else if ( com_timescale->value ) {
		usec *= com_timescale->value;
	}
	
	// don't let it scale below 1 usec
	if ( usec < 1 && com_timescale->value) {
		usec = 1;
	}

	if ( com_dedicated->integer ) {
		// dedicated servers don't want to clamp for a much longer
		// period, because it would mess up all the client's views
		// of time.
		if (usec > 500000)
			Com_Printf( "^5Hitch warning: %i msec frame time\n", usec / 1000 );

		clampTime = 5000000;
	} else if ( !com_sv_running->boolean ) {
		// clients of remote servers do not want to clamp time, because
		// it would skew their view of the server's time temporarily
		clampTime = 5000000;
	} else {
		// for local single player gaming
		// we may want to clamp the time to prevent players from
		// flying off edges when something hitches.
		clampTime = 200000;
	}

	if ( usec > clampTime ) {
		usec = clampTime;
	}

	return usec;
}

static time_t realtime;

time_t Com_GetRealtime()
{
	return realtime;
}

void Com_UpdateRealtime()
{
	time(&realtime);
}

/*
=================
Com_Frame
=================
*/
__optimize3 void Com_Frame( void ) {

	unsigned int			usec;
	static unsigned long long	lastTime;
	static unsigned int		com_frameNumber;


        jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(2);

        if(setjmp(*abortframe)){

            Sys_EnterCriticalSection(2);

            if(com_errorEntered)
                Com_ErrorCleanup();

            Sys_LeaveCriticalSection(2);
        }
	//
	// main event loop
	//
#ifdef TIMEDEBUG

	int		timeBeforeFirstEvents = 0;
	int		timeBeforeServer = 0;
	int		timeBeforeEvents = 0;
	int		timeBeforeClient = 0;
	int		timeAfter = 0;


	if ( com_speeds->integer ) {
		timeBeforeFirstEvents = Sys_Milliseconds ();
	}
#endif
	// Figure out how much time we have
//	if(com_dedicated->integer)
//		minUsec = SV_FrameUsec();
/*	else
	{
		if(com_minimized->integer && com_maxfpsMinimized->integer > 0)
			minMsec = 1000 / com_maxfpsMinimized->integer;
		else if(com_unfocused->integer && com_maxfpsUnfocused->integer > 0)
			minMsec = 1000 / com_maxfpsUnfocused->integer;
		else if(com_maxfps->integer > 0)
			minMsec = 1000 / com_maxfps->integer;
		else
			minMsec = 1;
		
		timeVal = com_frameTime - lastTime;
		bias += timeVal - minMsec;
		
		if(bias > minMsec)
			bias = minMsec;
		
		// Adjust minMsec if previous frame took too long to render so
		// that framerate is stable at the requested value.
		minMsec -= bias;
	}*/
//	timeVal = Com_TimeVal(minUsec);


/*	do
	{
		if(timeVal < 1){
			NET_Sleep(0);
		}else{
			NET_Sleep((timeVal - 1));
		}
		timeVal = Com_TimeVal(minUsec);
	} while( timeVal );
*/

	com_frameTime = Sys_MillisecondsLong();
	com_uFrameTime = Sys_MicrosecondsLong();

	usec = com_uFrameTime - lastTime;
	lastTime = com_uFrameTime;

	// mess with msec if needed
	usec = Com_ModifyUsec(usec);


	//
	// server side
	//
#ifdef TIMEDEBUG
	if ( com_speeds->integer ) {
		timeBeforeServer = Sys_Milliseconds ();
	}
#endif
	if(!SV_Frame( usec ))
		return;

	PHandler_Event(PLUGINS_ONFRAME);

	Com_TimedEventLoop();
	Com_EventLoop();
	Cbuf_Execute (0 ,0);
	NET_Sleep(0);
	NET_TcpServerPacketEventLoop();
	Cbuf_Execute (0 ,0);

	SetAnimCheck(com_animCheck->boolean);

#ifdef TIMEDEBUG
	if ( com_speeds->integer ) {
		timeAfter = Sys_Milliseconds ();
		timeBeforeEvents = timeAfter;
		timeBeforeClient = timeAfter;
	}
#endif

//	NET_FlushPacketQueue();
#ifdef TIMEDEBUG
	//
	// report timing information
	//
	if ( com_speeds->integer ) {
		int			all, sv, ev, cl;

		all = timeAfter - timeBeforeServer;
		sv = timeBeforeEvents - timeBeforeServer;
		ev = timeBeforeServer - timeBeforeFirstEvents + timeBeforeClient - timeBeforeEvents;
		cl = timeAfter - timeBeforeClient;
		sv -= time_game;
		cl -= time_frontend + time_backend;

		Com_Printf ("frame:%i all:%3i sv:%3i ev:%3i cl:%3i gm:%3i rf:%3i bk:%3i\n", 
					 com_frameNumber, all, sv, ev, cl, time_game, time_frontend, time_backend );
	}	
#endif
	//
	// trace optimization tracking
	//
#ifdef TRACEDEBUG
	if ( com_showtrace->integer ) {
	
		extern	int c_traces, c_brush_traces, c_patch_traces;
		extern	int	c_pointcontents;

		Com_Printf ("%4i traces  (%ib %ip) %4i points\n", c_traces,
			c_brush_traces, c_patch_traces, c_pointcontents);
		c_traces = 0;
		c_brush_traces = 0;
		c_patch_traces = 0;
		c_pointcontents = 0;
	}
#endif
	com_frameNumber++;

	Com_UpdateRealtime();

        Sys_EnterCriticalSection(2);

        if(com_errorEntered)
            Com_ErrorCleanup();

        Sys_LeaveCriticalSection(2);
}
