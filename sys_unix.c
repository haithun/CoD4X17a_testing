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
#include <libgen.h>

#define sys_timeBaseInt_ADDR 0x1411c5c4
#define sys_timeBaseInt *((int*)(sys_timeBaseInt_ADDR))


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
Sys_Dirname
==================
*/
const char *Sys_Dirname( char *path )
{
	return dirname( path );
}


/*
==================
Sys_Cwd
==================
*/
char *Sys_Cwd( void )
{
	static char cwd[MAX_OSPATH];

	char *result = getcwd( cwd, sizeof( cwd ) - 1 );
	if( result != cwd )
		return NULL;

	cwd[MAX_OSPATH-1] = 0;

	return cwd;
}

#define ELF_TYPEOFFSET 16
#define DLLMOD_FILESIZE 2281820

qboolean Sys_LoadImagePrepareFile(const char* path)
{
        FILE* fp;
        int rval, trys;
        char cmdline[MAX_OSPATH];
        char copypath[MAX_OSPATH];
        const char* dir;

        if(path == NULL)
            return qfalse;

        //Get directory name
        Q_strncpyz(copypath, path, sizeof(copypath)); //Copy 1st because the behaviour of dirname()is undefined
        dir = Sys_Dirname( copypath );

        //Test directory permissions:
        if(access(dir, F_OK) != 0)
        {
            printf("Error directory %s seems not to exist: %s\n", dir, strerror(errno));
            return qfalse;
        }

        if(access(dir, R_OK) != 0)
        {
            printf("Read access to directory %s is denied: %s\n", dir, strerror(errno));
            return qfalse;
        }

        if(access(dir, W_OK) != 0)
        {
            printf("Write access to directory %s is denied: %s\n", dir, strerror(errno));
            return qfalse;
        }

        trys = 0;

        if(access(path, F_OK) != 0)
        {
            printf("The file %s seems not to exist\n", path);

        dl_again:
            printf("Trying to download...\n");

            Com_sprintf(cmdline, sizeof(cmdline), "wget -O %s %s", path, "http://update.iceops.in/cod4_lnxded.so");
            rval = system( cmdline );
            if(rval != 0)
            {
                printf("Failed to download cod4_lnxded.so\nPlease make sure you are connected to the internet or install this file manually: %s\n", path);
                return qfalse;
            }

            if(access(path, F_OK) != 0)
            {
                printf("Failed to install cod4_lnxded.so\nPlease try to install this file manually: %s\n", path);
                return qfalse;

            }
        }

        if(access(path, R_OK) != 0)
        {
            printf("Read access to file %s is denied: %s\n", path, strerror(errno));
            return qfalse;
        }

        if(access(path, W_OK) != 0)
        {
            printf("Write access to file %s is denied: %s\n", path, strerror(errno));
            return qfalse;
        }

        //Test if it is the correct file and see if it is already a shared object
        fp = fopen(path, "rb");
        if(fp)
        {
            if( !fseek(fp, 0, SEEK_END) && ftell(fp) == DLLMOD_FILESIZE && !fseek(fp, ELF_TYPEOFFSET, SEEK_SET))
            {
                if(fgetc(fp) == 3)
                { //The elf type is shared library already
                    fclose(fp);
                    return qtrue;
                }
                //The elf type is exe file - we have to make it a shared library
                fclose(fp);

            }else{
                //The file can not be read or the size is wrong
                fclose(fp);
                printf("The file %s can not be read or has a wrong size.\n", path);
                if(trys < 1)
                {
                    printf("Deleting file: %s\n", path);
                    if(remove(path) != 0)
                    {
                        printf("Couldn't delete file %s Error: %s\n", path, strerror(errno));
                        return qfalse;
                    }
                    trys++;
                    goto dl_again;
                }
                return qfalse;
            }

        }else{
            printf("Failed to open file %s for reading - Error: %s\n", path, strerror(errno));
            return qfalse;
        }

        //Try to make it a shared object
        fp = fopen(path, "rb+");
        if(fp)
        {
            if(fseek(fp, ELF_TYPEOFFSET, SEEK_SET) != 0)
            {
                printf("Seek error on file %s opened for writing - Error: %s\n", path, strerror(errno));
                fclose(fp);
                return qfalse;
            }

            if(fputc(3, fp) == 3)
            {
                fclose(fp);
                return qtrue;
            }

            printf("Failed to write to file %s - Error: %s\n", path, strerror(errno));
            fclose(fp);
            return qfalse;
        }

        printf("Failed to open file %s for writing - Error: %s\n", path, strerror(errno));
        return qfalse;

}




/*
=============
Sys_LoadImage

=============
*/
void Sys_LoadImage( ){

    void *dl;
    char *error;
    char module[MAX_OSPATH];

    Com_sprintf(module, sizeof(module), "%s/%s", Sys_BinaryPath(), COD4_DLL);

    if(!Sys_LoadImagePrepareFile( module ))
    {
        printf("An error has occurred. Exiting...\n");
        _exit(1);
    }

    dl = dlopen(module, RTLD_LAZY);

    if(dl == NULL)
    {
        error = dlerror();
        printf("Failed to load required module: %s Error: %s\n", module, error);
        _exit(1);

    }
    printf("Loaded module: %s\n", module);

    /* No retrieving of symbols where none are :( */

    if(!Sys_PatchImage())
    {
        printf("Failed to patch module: %s\n", module);
        _exit(1);
    }
}


void Sys_InitCrashDumps(){

        // core dumps may be disallowed by parent of this process; change that

        struct rlimit core_limit;
        core_limit.rlim_cur = RLIM_INFINITY;
        core_limit.rlim_max = RLIM_INFINITY;

        if (setrlimit(RLIMIT_CORE, &core_limit) < 0)
            Com_PrintWarning("setrlimit: %s\nCore dumps may be truncated or non-existant\n", strerror(errno));

}

qboolean Sys_MemoryProtectWrite(void* startoffset, int len)
{

	if(mprotect(startoffset + getpagesize() - ((int)startoffset % getpagesize()), len - (len % getpagesize()), PROT_READ | PROT_WRITE) != 0)
	{
            perror("Sys_MemoryProtectWrite: mprotect change memory to writable error");
            return qfalse;
	}

	return qtrue;
}

qboolean Sys_MemoryProtectExec(void* startoffset, int len)
{

	if(mprotect(startoffset + getpagesize() - ((int)startoffset % getpagesize()), len - (len % getpagesize()), PROT_READ | PROT_EXEC) != 0)
	{
            perror("Sys_MemoryProtectExec: mprotect change memory to readonly/execute error");
            return qfalse;
	}

	return qtrue;
}

qboolean Sys_MemoryProtectReadonly(void* startoffset, int len)
{

	if(mprotect(startoffset + getpagesize() - ((int)startoffset % getpagesize()), len - (len % getpagesize()), PROT_READ) != 0)
	{
            perror("Sys_MemoryProtectReadonly: mprotect change memory to readonly error");
            return qfalse;
	}

	return qtrue;
}

static char homePath[MAX_OSPATH];

/*
==================
Sys_DefaultHomePath
==================
*/


const char *Sys_DefaultHomePath(void)
{
	char *p;

	if( !*homePath )
	{
		if( ( p = getenv( "HOME" ) ) != NULL )
		{
			Com_sprintf(homePath, sizeof(homePath), "%s%c", p, PATH_SEP);
#ifdef MACOS_X
			Q_strcat(homePath, sizeof(homePath),
				"Library/Application Support/");

			Q_strcat(homePath, sizeof(homePath), HOMEPATH_NAME_MACOSX);
#else
			Q_strcat(homePath, sizeof(homePath), HOMEPATH_NAME_UNIX);
#endif
		}
	}

	return homePath;
}

#ifndef MACOS_X


/*
================
Sys_TempPath
================
*/

const char *Sys_TempPath( void )
{
	const char *TMPDIR = getenv( "TMPDIR" );

	if( TMPDIR == NULL || TMPDIR[ 0 ] == '\0' )
		return "/tmp";
	else
		return TMPDIR;
}
#endif

