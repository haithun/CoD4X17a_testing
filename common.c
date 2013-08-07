#define	MAX_CONSOLE_LINES	32

#define cvar_modifiedFlags_ADDR 0x1402c060
#define com_codeTimeScale_ADDR 0x88a7238
#define com_frameTime_ADDR 0x88a61e0
#define com_lastError_ADDR 0x88a6220
#define com_errorEntered_ADDR 0x88a61f4
#define com_numConsoleLines_ADDR 0x88a7360
#define logfile_ADDR 0x88a6210
#define level_ADDR 0x8370440


#define level (*((level_locals_t*)(level_ADDR)))
#define com_errorEntered *((qboolean*)(com_errorEntered_ADDR))
#define com_frameTime *((unsigned int*)(com_frameTime_ADDR))
#define com_codeTimeScale *((int*)(com_codeTimeScale_ADDR))
#define cvar_modifiedFlags *((int*)(cvar_modifiedFlags_ADDR))
#define com_lastError ((char*)com_lastError_ADDR)

jmp_buf		*abortframe;
int		com_numConsoleLines;
int		com_frameNumber;
char		*com_consoleLines[MAX_CONSOLE_LINES];
fileHandle_t	logfile;
fileHandle_t	adminlogfile;
fileHandle_t	enterleavelogfile;
fileHandle_t	reliabledump;


unsigned long long com_uFrameTime = 0;

qboolean	loadconfigfiles; //Needed for adminlogfile to omit logging of some actions while configfiles are loaded

cvar_t* com_version;
cvar_t* com_shortversion;
cvar_t* com_dedicated;
cvar_t* com_ansiColor;
cvar_t* com_timescale;
cvar_t* com_fixedtime;
cvar_t* com_maxFrameTime;
cvar_t* com_animCheck;


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

	PbCapatureConsoleOutput(msg, MAXPRINTMSG);
	if(dumbIWvar == 6) return;

	int msglen = strlen(msg);

	Sys_EnterCriticalSection(5);

	if ( type != MSG_NORDPRINT) {
		HL2Rcon_SourceRconSendConsole( msg, msglen);

		Com_PrintUDP( msg, msglen );

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
	    if ( logfile && FS_Initialized()) {
	    	FS_Write(msg, msglen, logfile);
	    }
	}
	Sys_LeaveCriticalSection(5);
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
		
	if ( !com_developer || !com_developer->integer ) {
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

	if ( !com_developer || !com_developer->integer ) {
		return;			// don't confuse non-developers with techie stuff...
	}
	
	msg[0] = '^';
	msg[1] = '2';

	va_start (argptr,fmt);	
	Q_vsnprintf (&msg[2], (sizeof(msg)-3), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( 0, msg, MSG_NORDPRINT);
}



/*
========================================================================

UDP logging facility

========================================================================
*/

#define MAX_LOGADDRESSES 8

netadr_t com_logaddresses[MAX_LOGADDRESSES];



static void Com_LogStreaming_f( void ){

	if(Cmd_Argc() < 2){

		if(com_logfile->integer)
			Com_Printf("Usage:  log < on | off >\ncurrently logging to: file, console\n");
		else
			Com_Printf("Usage:  log < on | off >\ncurrently logging to: console\n");
	}

}

static void Com_SetLogAddress_f( void ){

	int i;
	netadr_t *adr;
	netadr_t *freeadr = NULL;
	netadr_t adrtoadd;

	if(Cmd_Argc() < 2 || NET_StringToAdr(Cmd_Argv(1), &adrtoadd, NA_UNSPEC) != 1){
		Com_Printf("Usage:  logaddress_add < address:port >\n");
		return;
	}

	for(i = 0, adr = com_logaddresses ; i < MAX_LOGADDRESSES; i++, adr++)
	{
		if(NET_CompareAdr(adr, &adrtoadd)){
			Com_Printf("logaddress_add:    This address exists already added\n");
			return;
		}

		if(!freeadr && adr->type == NA_BAD){
			freeadr = adr;
		}
	}
	if(!freeadr){
		Com_Printf("logaddress_add:  Too many logaddresses. Limit is 8\n");
		return;
	}
	*freeadr = adrtoadd;
	Com_Printf("logaddress_add:  %s\n", NET_AdrToString(&adrtoadd));
}


static void Com_RemoveLogAddress_f( void ){

	int i;
	netadr_t *adr;
	netadr_t adrtodel;

	if(Cmd_Argc() < 2 || NET_StringToAdr(Cmd_Argv(1), &adrtodel, NA_UNSPEC) != 1){
		Com_Printf("Usage:  logaddress_del < address:port >\n");
	}


	for(i = 0, adr = com_logaddresses ; i < MAX_LOGADDRESSES; i++, adr++)
	{
		if(adr->type == NA_BAD)
			continue;

		if(NET_CompareAdr(adr, &adrtodel)){
			adr->type = NA_BAD;
			Com_Printf("logaddress_del:  %s\n", NET_AdrToString(&adrtodel));
			return;
		}
	}
	Com_Printf("logaddress_del:  No such address\n");
}


static void Com_RemoveAllLogAddresses_f( void ){

	int i, j;
	netadr_t *adr;

	Com_Printf("logaddress_delall:\n");

	for(i = 0, j = 0, adr = com_logaddresses ; i < MAX_LOGADDRESSES; i++, adr++)
	{
		if(adr->type == NA_BAD)
			continue;

		j++;
		Com_Printf("%d:    %s\n", j, NET_AdrToString(adr));
		adr->type = NA_BAD;
	}
	Com_Printf("------------------\n");
}


static void Com_ListAllLogAddresses_f( void ){

	int i, j;
	netadr_t *adr;

	Com_Printf("logaddress_list:\n");

	for(i = 0, j = 0, adr = com_logaddresses ; i < MAX_LOGADDRESSES; i++, adr++)
	{
		if(adr->type == NA_BAD)
			continue;

		j++;
		Com_Printf("%d:    %s\n", j, NET_AdrToString(adr));
	}
	Com_Printf("------------------\n");
}


void Com_AddLoggingCommands(){

	static qboolean	initialized;

	if ( initialized ) {
		return;
	}
	initialized = qtrue;

	Cmd_AddCommand ("logaddress_delall", Com_RemoveAllLogAddresses_f);
	Cmd_AddCommand ("logaddress_list", Com_ListAllLogAddresses_f);
	Cmd_AddCommand ("logaddress_add", Com_SetLogAddress_f);
	Cmd_AddCommand ("logaddress_del", Com_RemoveLogAddress_f);
	Cmd_AddCommand ("log", Com_LogStreaming_f);
}


void Com_PrintUDP( const char* msg, int len ){

	int i;
	netadr_t *adr;

	for(i = 0, adr = com_logaddresses ; i < MAX_LOGADDRESSES; i++, adr++)
	{
		if(adr->type == NA_BAD)
			continue;

		NET_OutOfBandData(NS_SERVER, adr, (byte*)msg, len);
	}
}



/*
========================================================================

EVENT LOOP

========================================================================
*/


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
===============
Com_StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets should
be after execing the config and default.
===============
*/
void Com_StartupVariable( const char *match ) {
	int		i;
	for (i=0 ; i < com_numConsoleLines ; i++) {
		Cmd_TokenizeString( com_consoleLines[i] );

		if(!match || !strcmp(Cmd_Argv(1), match))
		{
			if ( !strcmp( Cmd_Argv(0), "set" )){
				Cvar_Set_f();
				Cmd_EndTokenizeString();
				continue;
			}else if( !strcmp( Cmd_Argv(0), "seta" ) ) {
				Cvar_SetA_f();
			}
		}
		Cmd_EndTokenizeString();
	}
}




/*
=================
Com_AddStartupCommands

Adds command line parameters as script statements
Commands are seperated by + signs

Returns qtrue if any late commands were added, which
will keep the demoloop from immediately starting
=================
*/
qboolean Com_AddStartupCommands( void ) {
	int		i;
	qboolean	added;
	char		buf[1024];
	added = qfalse;
	// quote every token, so args with semicolons can work
	for (i=0 ; i < com_numConsoleLines ; i++) {
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] ) {
			continue;
		}

		// set commands already added with Com_StartupVariable
		if ( !Q_stricmpn( com_consoleLines[i], "set", 3 )) {
			continue;
		}

		added = qtrue;
		Com_sprintf(buf,sizeof(buf),"%s\n",com_consoleLines[i]);
		Cbuf_ExecuteBuffer( 0,0, buf);
	}

	return added;
}



/*
==================
Com_ParseCommandLine

Break it up into multiple console lines
==================
*/
void Com_ParseCommandLine( char *commandLine ) {
    int inq = 0;
    com_consoleLines[0] = commandLine;
    com_numConsoleLines = 1;

    while ( *commandLine ) {
        if (*commandLine == '"') {
            inq = !inq;
        }
        // look for a + seperating character
        // if commandLine came from a file, we might have real line seperators
        if ( (*commandLine == '+' && !inq) || *commandLine == '\n'  || *commandLine == '\r' ) {
            if ( com_numConsoleLines == MAX_CONSOLE_LINES ) {
                return;
            }
            com_consoleLines[com_numConsoleLines] = commandLine + 1;
            com_numConsoleLines = (com_numConsoleLines)+1;
            *commandLine = 0;
        }
        commandLine++;
    }
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
		SV_DemoSystemShutdown();
		Hunk_ClearTempMemory();
		Hunk_ClearTempMemoryHigh();
		SV_Shutdown("EXE_SERVERQUIT");

		Com_Close();

		if(logfile) FS_FCloseFile(logfile);
		if(adminlogfile) FS_FCloseFile(adminlogfile);
		if(enterleavelogfile) FS_FCloseFile(enterleavelogfile);
		if(reliabledump) FS_FCloseFile(reliabledump);

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
    com_ansiColor = Cvar_RegisterBool ("ttycon_ansiColor", qtrue, CVAR_ARCHIVE , "Use AnsiColors");

    cvar_t** tmp;
    tmp = (cvar_t**)(0x88a6170);
    *tmp = Cvar_RegisterBool ("useFastFils", qtrue, 16, "Enables loading data from fast files");
    //MasterServer
    //AuthServer
    //MasterServerPort
    //AuthServerPort
    tmp = (cvar_t**)(0x88a6184);
    *tmp = Cvar_RegisterInt ("developer", 0, 0, 2, 0, "Enable development options");
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

    abortframe = Sys_GetValue(2);
    if(setjmp(*abortframe)){
        Sys_Error(va("Error during Initialization:\n%s\n", com_lastError));
    }
    Com_Printf("%s %s %s build %i %s\n", GAME_STRING,Q3_VERSION,PLATFORM_STRING, BUILD_NUMBER, __DATE__);

    patch_assets();  //Patch several asset-limits to higher values

    SL_Init();

    Swap_Init();

    Cbuf_Init();

    Cmd_Init();

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

    Com_AddLoggingCommands();
    HL2Rcon_AddSourceAdminCommands();
    Cmd_AddCommands();

    Sys_Init();

    Com_RandomBytes( (byte*)&qport, sizeof(int) );
    Netchan_Init( qport );

    Scr_InitVariables();

    Scr_Init(); //VM_Init

    Scr_Settings(com_logfile->integer || com_developer->integer ,com_developer_script->integer, com_developer->integer);

    XAnimInit();

    DObjInit();

    Plugin_Init();

    NET_Init();

    SV_Init();

    com_frameTime = Sys_Milliseconds();

    if(useFastfiles->integer){
        Mem_EndAlloc("$init", qtrue);
        DB_SetInitializing( qfalse );
        Com_Printf("end $init %d ms\n", Sys_Milliseconds() - msec);

        int XAssetscount;
        XZoneInfo XZoneInfoStack[6];

        XZoneInfoStack[4].fastfile = "localized_common_mp";
        XZoneInfoStack[4].loadpriority = 1;
        XZoneInfoStack[4].notknown = 0;
        XZoneInfoStack[3].fastfile = "common_mp";
        XZoneInfoStack[3].loadpriority = 4;
        XZoneInfoStack[3].notknown = 0;
        XZoneInfoStack[2].fastfile = "ui_mp";
        XZoneInfoStack[2].loadpriority = 8;
        XZoneInfoStack[2].notknown = 0;
        XZoneInfoStack[1].fastfile = "localized_code_post_gfx_mp";
        XZoneInfoStack[1].loadpriority = 0;
        XZoneInfoStack[1].notknown = 0;
        XZoneInfoStack[0].fastfile = "code_post_gfx_mp";
        XZoneInfoStack[0].loadpriority = 2;
        XZoneInfoStack[0].notknown = 0;

        if(DB_ModFileExists()){
            XAssetscount = 6;
            XZoneInfoStack[5].fastfile = "mod";
            XZoneInfoStack[5].loadpriority = 16;
            XZoneInfoStack[5].notknown = 0;
        }else{
            XAssetscount = 5;
        }
        DB_LoadXAssets(&XZoneInfoStack[0],XAssetscount,0);
    }



    Com_DvarDump(6,0);

    NV_LoadConfig();

    time(&realtime);

    Com_Printf("--- Common Initialization Complete ---\n");

    Cbuf_Execute( 0, 0 );

    Com_AddStartupCommands( );

    abortframe = Sys_GetValue(2);
    if(setjmp(*abortframe)){
        Sys_Error(va("Error during Initialization:\n%s\n", com_lastError));
    }
    if(com_errorEntered) Com_ErrorCleanup();
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

/*
=================
Com_Frame
=================
*/
__optimize3 void Com_Frame( void ) {

	unsigned int		usec;
	unsigned long long	lastTime;

        abortframe = Sys_GetValue(2);
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

	lastTime = com_uFrameTime;

	struct timeval tp;
	gettimeofday( &tp, NULL );

	unsigned int diffseconds = tp.tv_sec - sys_timeBase;

	com_frameTime = diffseconds * 1000 + tp.tv_usec / 1000;

	unsigned long long orgtime = diffseconds;

	com_uFrameTime =  orgtime << 19;
	com_uFrameTime += orgtime << 18;
	com_uFrameTime += orgtime << 17;
	com_uFrameTime += orgtime << 16;
	com_uFrameTime += orgtime << 14;
	com_uFrameTime += orgtime << 9;
	com_uFrameTime += orgtime << 6;
	com_uFrameTime += (unsigned long long)tp.tv_usec;

	usec = com_uFrameTime - lastTime;

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

	time(&realtime);

	Plugin_Event(PLUGINS_ONFRAME);

	Com_TimedEventLoop();
	Com_EventLoop();
	Cbuf_Execute (0 ,0);
	NET_Sleep(0);
	NET_TcpPacketEventLoop();
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

        Sys_EnterCriticalSection(2);
        if(com_errorEntered)
            Com_ErrorCleanup();
        Sys_LeaveCriticalSection(2);
}




char* SL_ConvertToString(unsigned int index){

    char** ptr = (char**)STRBUFFBASEPTR_ADDR;
    char* base = *ptr;
    return &base[ index*12 + 4];
}



/*
=================
Com_ReadTcpPackets
=================
*/
__optimize3 __regparm3 tcpclientstate_t Com_TcpAuthPacketEvent( netadr_t *from, msg_t *msg, int *socketfd, int *connectionId, int *serviceId ){

	tcpclientstate_t ret;

	Com_DPrintf("Packet event from: %s\n", NET_AdrToString(from));

	ret = HL2Rcon_SourceRconAuth(from, msg, socketfd, connectionId);
	if(ret != TCP_AUTHNOTME){
		*serviceId = 0x782a3;
		return ret;
	}

	Com_DPrintf("^5Bad TCP-Packet from: %s\n", NET_AdrToString(from));
	return TCP_AUTHBAD;
}

__optimize3 __regparm3 qboolean Com_TcpPacketEvent( netadr_t *from, msg_t *msg, int *socketfd, int connectionId, int serviceId ){

	if(serviceId == 0x782a3){
		return HL2Rcon_SourceRconEvent(from, msg, socketfd, connectionId);
	}

	Com_PrintError("Com_TcpPacketEvent: Bad serviceId: %d\n", serviceId);
	return qtrue;

}

__optimize3 __regparm3 void Com_TcpConnectionClosed( netadr_t *from, int socketfd, int connectionId, int serviceId ){

	if(serviceId == 0x782a3){
		HL2Rcon_SourceRconDisconnect(from, socketfd, connectionId);
		return;
	}
	Com_PrintError("Com_TcpConnectionClosed: Bad serviceId: %d\n", serviceId);
}


