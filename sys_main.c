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

#include <sys/resource.h>

#define MAX_QUED_EVENTS 256
sysEvent_t *eventQue;
#define evenQue_ADDR 0x14118720
#define sys_timeBaseInt_ADDR 0x1411c5c4

#define sys_timeBaseInt *((int*)(sys_timeBaseInt_ADDR))

#define MAX_CMD 1024
static char exit_cmdline[MAX_CMD] = "";


/*
==================
Sys_RandomBytes
==================
*/
qboolean Sys_RandomBytes( byte *string, int len )
{
	FILE *fp;

	fp = fopen( "/dev/urandom", "r" );
	if( !fp )
		return qfalse;

	if( !fread( string, sizeof( byte ), len, fp ) )
	{
		fclose( fp );
		return qfalse;
	}

	fclose( fp );
	return qtrue;
}

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

int Sys_Milliseconds( void ) {
	struct timeval tp;

	gettimeofday( &tp, NULL );

	return ( tp.tv_sec - sys_timeBase ) * 1000 + tp.tv_usec / 1000;
}


void Sys_TimerInit( void ) {
	struct timeval tp;

	gettimeofday( &tp, NULL );

	if ( !sys_timeBase ) {
		sys_timeBase = tp.tv_sec;
		sys_timeBaseInt = tp.tv_sec;
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



void Sys_DoStartProcess( char *cmdline ) {
	switch ( fork() )
	{
	case - 1:
		// main thread
		break;
	case 0:
		if ( strchr( cmdline, ' ' ) ) {
			system( cmdline );

		} else {
			execl( cmdline, cmdline, NULL );
			printf( "execl failed: %s\n", strerror( errno ) );

		}
		_exit( 0 );
		break;
	}
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
	//  as long as GL DLL's keep installing with atexit...
	//exit(ex);
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
		if(logfile)
			FS_FCloseFile(logfile);
		if(adminlogfile)
			FS_FCloseFile(adminlogfile);
		if(reliabledump)
			FS_FCloseFile(reliabledump);

		FS_Shutdown(qtrue);
	}

	if( signal == SIGTERM || signal == SIGINT )
		Sys_Exit( 1 );
	else
		Sys_Exit( 2 );
}


void Sys_TermProcess(int signal)
{
    int status;
    wait(&status);
    return;
}



/*
==============
Sys_PlatformInit

Unix specific initialisation
==============
*/
void Sys_PlatformInit( void )
{
	const char* term = getenv( "TERM" );

	signal( SIGHUP, Sys_SigHandler );
	signal( SIGQUIT, Sys_SigHandler );
	signal( SIGTRAP, Sys_SigHandler );
	signal( SIGIOT, Sys_SigHandler );
	signal( SIGBUS, Sys_SigHandler );
//	signal( SIGCHLD, Sys_TermProcess );

	stdinIsATTY = isatty( STDIN_FILENO ) &&
		!( term && ( !strcmp( term, "raw" ) || !strcmp( term, "dumb" ) ) );
}



/*
==================
Sys_DoStartProcess
actually forks and starts a process

UGLY HACK:
  Sys_StartProcess works with a command line only
  if this command line is actually a binary with command line parameters,
  the only way to do this on unix is to use a system() call
  but system doesn't replace the current process, which leads to a situation like:
  wolf.x86--spawned_process.x86
  in the case of auto-update for instance, this will cause write access denied on wolf.x86:
  wolf-beta/2002-March/000079.html
  we hack the implementation here, if there are no space in the command line, assume it's a straight process and use execl
  otherwise, use system ..
  The clean solution would be Sys_StartProcess and Sys_StartProcess_Args..
==================
*/


void Sys_PrintBinVersion( const char* name ) {
	char* sep = "==============================================================";
	fprintf( stdout, "\n\n%s\n", sep );

	fprintf( stdout, "%s %s %s %s\n", GAME_STRING, Q3_VERSION, PLATFORM_STRING, __DATE__ );

	fprintf( stdout, " local install: %s\n", name );
	fprintf( stdout, "%s\n\n", sep );
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




/*
=================
Sys_AnsiColorPrint

Transform Q3 colour codes to ANSI escape sequences
=================
*/
void Sys_AnsiColorPrint( const char *msg )
{
	static char buffer[ MAXPRINTMSG ];
	int         length = 0;
	static int  q3ToAnsi[ 8 ] =
	{
		30, // COLOR_BLACK
		31, // COLOR_RED
		32, // COLOR_GREEN
		33, // COLOR_YELLOW
		34, // COLOR_BLUE
		36, // COLOR_CYAN
		35, // COLOR_MAGENTA
		0   // COLOR_WHITE
	};

	while( *msg )
	{
		if( Q_IsColorString( msg ) || *msg == '\n' )
		{
			// First empty the buffer
			if( length > 0 )
			{
				buffer[ length ] = '\0';
				fputs( buffer, stderr );
				length = 0;
			}

			if( *msg == '\n' )
			{
				// Issue a reset and then the newline
				fputs( "\033[0m\n", stderr );
				msg++;
			}
			else
			{
				// Print the color code
				Com_sprintf( buffer, sizeof( buffer ), "\033[1;%dm",
						q3ToAnsi[ ColorIndex( *( msg + 1 ) ) ] );
				fputs( buffer, stderr );
				msg += 2;
			}
		}
		else
		{
			if( length >= MAXPRINTMSG - 1 )
				break;

			buffer[ length ] = *msg;
			length++;
			msg++;
		}
	}

	// Empty anything still left in the buffer
	if( length > 0 )
	{
		buffer[ length ] = '\0';
		fputs( buffer, stderr );
	}
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

	fdout=fopen(fileout, "w");
	if(fdout){
		sprintf(buffer, "Sys_Error: %s\n", msg);
		fwrite(buffer, strlen(buffer), 1 ,fdout);
		fclose(fdout);
	}
        Sys_Error_real(msg);
}


#define TEXT_SECTION_OFFSET 0x2c20
#define TEXT_SECTION_LENGTH 0x1bf1a4
#define RODATA_SECTION_OFFSET 0x1c1e00
#define RODATA_SECTION_LENGTH 0x36898
#define DATA_SECTION_OFFSET 0x222580
#define DATA_SECTION_OFFSET_FIX 0x1000
#define DATA_SECTION_LENGTH 0x9454
#define IMAGE_BASE 0x8048000

/*
=============
Sys_LoadDifferentImage

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void Sys_LoadDifferentImage(char* argv[]){

	FILE * f;
//	int flen;
	char* filein = "update.img";
//	void*		buffer;

	f = fopen(filein, "rb");
	if(!f)
		return;
	//File there, start the image update
/*
	fseek(f, 0, SEEK_END);
	flen = ftell(f);
	fseek(f, 0L, SEEK_SET);

	buffer = malloc(flen);
	if(!buffer)
		return;

	if((fread (buffer,1,flen,f)) != flen)
	{
		printf("Failure by reading file: %s\n", filein);
		return;
	}*/
	fclose(f);

/*
	void* textbase = (void*)(TEXT_SECTION_OFFSET + IMAGE_BASE + (getpagesize() - TEXT_SECTION_OFFSET % getpagesize()));
	int textlen = TEXT_SECTION_LENGTH - (getpagesize() - TEXT_SECTION_OFFSET % getpagesize());

	if(mprotect(textbase, textlen, PROT_WRITE) != 0){
	    perror("mprotect change memory to writable error");
	    return;
	}

	memcpy(textbase, (buffer + (textbase - (void*)IMAGE_BASE)), textlen);


	if(mprotect(textbase, textlen, PROT_READ | PROT_EXEC) != 0){
	    perror("mprotect change memory to readonly and executable error");
	    _exit(0);
	}

	void* database = (void*)(DATA_SECTION_OFFSET + DATA_SECTION_OFFSET_FIX + IMAGE_BASE);
	int datalen = DATA_SECTION_LENGTH;

	memcpy( database, (buffer + (database - (void*)(IMAGE_BASE + DATA_SECTION_OFFSET_FIX))), datalen);
*/
	//Are we running already the modified image ?

	if((*(byte*)(0x807fa7c)) == 0xf4)
	    return;

        execvp("./update.img", argv);
        printf("Image loading failed\n");
}


void addrtest(){


/*
	struct gclient_s* gs = NULL;
	struct gentity_s* gnt = NULL;
	level_locals_t* ll = NULL;
	client_t* cl = NULL;
	usercmd_t* ucmd = NULL;
	playerState_t *ps = NULL;

	Com_Printf("Sizeof cl: %x\n", sizeof(client_t));


	Com_Printf("Address: %x\n",&cl->clscriptid);
	Com_Printf("Address: %x\n",&cl->serverId);*/
/*	Com_Printf("Address: %i\n",&ps->dofViewmodelEnd);
	Com_Printf("Address: %i\n",&ps->deltaTime);
	Com_Printf("Sizeof ps: %x\n", sizeof(playerState_t));



	Com_Printf("Address: %x\n",&gs->sess.sessionTeam);
	Com_Printf("Size: %x\n", sizeof(struct gclient_s));
	Com_Printf("Address: %x\n", &ll->voteNo);
	Com_Printf("Address: %x\n",&ucmd->buttons);
	Com_Printf("Address: %x\n",&scrStruct.var_10);
	Com_Printf("Address: %x\n",&sv.svEntities);
	Com_Printf("Address: %x\n",&test->eFlags);




	__asm("mov $0x0,%%eax\n"
	      "dec %%eax\n"
	      "xor %%edx,%%edx\n"
	      "cmp $0x40,%%eax\n"
	      "ja val_above\n"
	      "inc %%edx\n"
	      "val_above:\n"
	      :"=d"(test)::"eax");


	Com_Printf("Val: %i\n",test);*/
/*
	int test;
	int invar  = 2;

	__asm("cmp $3,%%eax\n"
	      "sbb %%eax,%%eax\n"
	      :"=a"(test) : "a"(invar));
	
	Com_Printf("val: %i\n", test);


	invar  = 1;

	__asm("cmp $3,%%eax\n"
	      "sbb %%eax,%%eax\n"
	      :"=a"(test):"a"(invar));
	
	Com_Printf("val: %i\n", test);

	invar  = 0;
	__asm("cmp $3,%%eax\n"
	      "sbb %%eax,%%eax\n"
	      :"=a"(test):"a"(invar));
	
	Com_Printf("val: %i\n", test);


	invar  = -1;
	__asm("cmp $3,%%eax\n"
	      "sbb %%eax,%%eax\n"
	      :"=a"(test):"a"(invar));
	
	Com_Printf("val: %i\n", test);

	invar  = -2;
	__asm("cmp $3,%%eax\n"
	      "sbb %%eax,%%eax\n"
	      :"=a"(test):"a"(invar));
	
	Com_Printf("val: %i\n", test);



*/
	_exit(0);
}



//void addrtest(){
	/*int i;
	char* str1 = "this is a 99 test string for 99 ballon out there here at 99th headquarter";
	char* str2 = "nintynine";

	char buf[256];
	int starttime = Sys_Milliseconds();
	
	for(i = 0; i < 200000; i++){
		Q_strnrepl(buf, sizeof(buf), str1, "99", str2);
	}
	int elapsedtime = Sys_Milliseconds() - starttime;
	
	Com_Printf("1 million events took: %i msec buf: %s\n", elapsedtime, buf);
	_exit(0);*/
/*	client_t *cl = NULL;
	Com_Printf("CLAddr: %p\n",&cl->unknownUsercmd1);
	_exit(0);*/
//}



void Sys_InitCrashDumps(){

        // core dumps may be disallowed by parent of this process; change that

        struct rlimit core_limit;
        core_limit.rlim_cur = RLIM_INFINITY;
        core_limit.rlim_max = RLIM_INFINITY;

        if (setrlimit(RLIMIT_CORE, &core_limit) < 0)
            Com_PrintWarning("setrlimit: %s\nCore dumps may be truncated or non-existant\n", strerror(errno));


}


DLL_PUBLIC __cdecl int main(int argc, char* argv[]){

    int len, i;
    char  *cmdline;
    eventQue = (sysEvent_t*)evenQue_ADDR;

    // go back to real user for config loads

    Sys_LoadDifferentImage(argv);

    seteuid(getuid());

    Sys_PlatformInit( );

    Sys_ThreadInit();

    Sys_ThreadMain();

    Com_InitParse();

//    Cvar_Init();



    Sys_ParseArgs(argc, argv);

    // merge the command line, this is kinda silly
    for ( len = 1, i = 1; i < argc; i++ )
        len += strlen( argv[i] ) + 1;

    cmdline = malloc( len );
    *cmdline = 0;
    for ( i = 1; i < argc; i++ ){
        if ( i > 1 ) {
            strcat( cmdline, " " );
        }
        strcat( cmdline, argv[i] );
    }

    // bk000306 - clear eventqueue
    memset( &eventQue[0], 0, MAX_QUED_EVENTS * sizeof( sysEvent_t ) );

    Sys_TimerInit(  );

//    addrtest();

    CON_Init();

    Sys_InitCrashDumps();

    Com_Init( cmdline );



    signal( SIGILL, Sys_SigHandler );
    signal( SIGFPE, Sys_SigHandler );
//    signal( SIGSEGV, Sys_SigHandler );
    signal( SIGTERM, Sys_SigHandler );
    signal( SIGINT, Sys_SigHandler );


//    GetPlayerFieldArray();



    fcntl( 0, F_SETFL, fcntl( 0, F_GETFL, 0 ) | FNDELAY );

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