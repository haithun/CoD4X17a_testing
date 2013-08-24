#include "q_shared.h"
#include "qcommon_io.h"
#include "sys_main.h"
#include "sys_cod4defs.h"
#include "sys_patch.h"
#include "g_sv_shared.h"
#include "punkbuster.h"
#include "sys_net.h"
#include "scr_vm.h"
#include "server.h"
#include "scr_vm_functions.h"
#include "sys_thread.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dlfcn.h>



#define ELF_TYPEOFFSET 16
#define DLLMOD_FILESIZE 2281820

static qboolean Sys_LoadImagePrepareFile(const char* path)
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





static void Sys_PatchImageData( void )
{


static byte patchblock_01[] = { 0xAE, 0xA, 0x5, 0x8, 
	0x89, 0x3C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90
};

static byte patchblock_Pmove_GetSpeed[] = { 0xC6, 0x5E, 0xA, 0x8,
	0x89, 0x3C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC
};

static byte patchblock_Pmove_GetGravity[] = { 0xC4, 0x68, 0xA, 0x8, 
	0x8B, 0x98, 0x5C, 0x1, 0x0, 0x0, 0x89, 0x5D, 0xB4, 0x89, 0x1C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC,
	0xCC, 0x89, 0x43, 0x58, 0x90, 0x90, 0xC7, 0x83, 0x9C, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x8B, 0x55, 0x8, 0xC6, 0x82, 0x6E, 0x1, 0x0, 0x0, 0xB, 0x8B, 0x4D, 0xB4
};

static byte patchblock_ClientConnect_NoPassword[] = { 0x76, 0x85, 0xA, 0x8,
	0xEB, 0x2C
};

static byte patchblock_G_RegisterCvars[] = { 0x57, 0x49, 0xB, 0x8,
	0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_Scr_AddSourceBuffer_Ignore_fs_game[] = { 0x74, 0xFC, 0x14, 0x8,
	0xEB, 0x4A
};

static byte patchblock_Scr_AddSourceBuffer_Ignore_fs_game2[] = { 0x22, 0xFD, 0x14, 0x8,
	0x89, 0xd0, 0x90
};

static byte patchblock_SV_SpawnServer[] = { 0x7E, 0x4A, 0x17, 0x8,
	0xeb, 0x19
};

static byte patchblock_SV_SendServerCommand[] = { 0x56, 0x74, 0x17, 0x8,
	0xeb, 0x5c
};
//NET_OutOfBandPrint prototype got changed from netadr_t to netadr_t* The remaining hooks should get fixed up by this:
static byte patchblock_NET_OOB_CALL1[] = { 0x75, 0x50, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xE8, 0x18, 0x23, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL2[] = { 0xB3, 0x69, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xEA, 0x86, 0x21, 0x8, 0x8B, 0x5D, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 
	0x24, 0x4, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL3[] = { 0x41, 0xB5, 0x17, 0x8,
	0xC7, 0x44, 0x24, 0x8, 0xEA, 0x86, 0x21, 0x8, 0x8D, 0x46, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

static byte patchblock_NET_OOB_CALL4[] = { 0x9B, 0x53, 0x17, 0x8,
	0x8D, 0x45, 0x9C, 0x89, 0x44, 0x24, 0x8, 0x8D, 0x43, 0x20, 0x89, 0x44, 0x24, 0x4, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};


	Sys_PatchImageWithBlock(patchblock_01, sizeof(patchblock_01));
	Sys_PatchImageWithBlock(patchblock_Pmove_GetSpeed, sizeof(patchblock_Pmove_GetSpeed));
	Sys_PatchImageWithBlock(patchblock_Pmove_GetGravity, sizeof(patchblock_Pmove_GetGravity));
	Sys_PatchImageWithBlock(patchblock_ClientConnect_NoPassword, sizeof(patchblock_ClientConnect_NoPassword));
	Com_Memset((void*)0x80b4872, 0x90, 41); //In G_RegisterCvars()
	Sys_PatchImageWithBlock(patchblock_G_RegisterCvars, sizeof(patchblock_G_RegisterCvars));
	Sys_PatchImageWithBlock(patchblock_Scr_AddSourceBuffer_Ignore_fs_game, sizeof(patchblock_Scr_AddSourceBuffer_Ignore_fs_game));  //Maybe script unlock ?
	Sys_PatchImageWithBlock(patchblock_Scr_AddSourceBuffer_Ignore_fs_game2, sizeof(patchblock_Scr_AddSourceBuffer_Ignore_fs_game2));  //Script unlock
	Sys_PatchImageWithBlock(patchblock_SV_SpawnServer, sizeof(patchblock_SV_SpawnServer));  //Skip useless check for cvar: sv_dedicated
	Com_Memset((void*)0x8174da9, 0x90, 5); //In SV_SpawnServer()  Don't overwrite sv.frameusec  (was before unknown write only variable)
	Com_Memset((void*)0x8174db5, 0x90, 42); //In SV_SpawnServer()  Don't set cvar cl_paused as well as nextmap
	Sys_PatchImageWithBlock(patchblock_SV_SendServerCommand, sizeof(patchblock_SV_SendServerCommand));  //Skip useless check for cvar: sv_dedicated
	Com_Memset((void*)0x8204acf, 0x90, 16); //In ???() Skip useless check for cvar: sv_dedicated
	Com_Memset((void*)0x8204ce9, 0x90, 16); //In ???() Skip useless check for cvar: sv_dedicated
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL1, sizeof(patchblock_NET_OOB_CALL1));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL2, sizeof(patchblock_NET_OOB_CALL2));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL3, sizeof(patchblock_NET_OOB_CALL3));
	Sys_PatchImageWithBlock(patchblock_NET_OOB_CALL4, sizeof(patchblock_NET_OOB_CALL4));

	SetCall(0x8050ab1, Jump_CalcHeight);
	SetJump(0x8050786, Jump_IsPlayerAboveMax);
	SetJump(0x80507c6, Jump_ClampVelocity);
	SetJump(0x805072a, Jump_GetStepHeight);
	SetCall(0x80a4b5f, StopFollowingOnDeath);
	SetJump(0x80a559e, StuckInClient);
	SetCall(0x80a5ec9, Pmove_GetSpeed);
	SetCall(0x80a68d0, Pmove_GetGravity);
	SetJump(0x80a7b60, ClientSpawn);
	SetJump(0x80ae962, G_Say);
	SetJump(0x80adbf2, Cmd_CallVote_f);
	SetCall(0x80b4957, G_RegisterCvarsCallback);
	SetJump(0x80c0b5a, GScr_LoadScripts);
	SetJump(0x80bc03e, ExitLevel); //ToDo Maybe build GScr_ExitLevel directly
	SetJump(0x810e6ea, PbSvGameQuery);
	SetJump(0x810e5dc, PbSvSendToClient);
	SetJump(0x810e5b0, PbSvSendToAddrPort);
	SetJump(0x817e988, SV_ClipMoveToEntity);
	SetJump(0x81d5a14, Sys_Error);
	SetJump(0x8122724, Com_PrintMessage);
	SetJump(0x8122d94, Com_DPrintfWrapper);
	SetJump(0x817452e, SV_Shutdown);
	SetJump(0x813d086, NET_OutOfBandPrint);
	SetJump(0x80bfef4, Scr_GetMethod);
	SetJump(0x80bd238, Scr_GetFunction);
	SetJump(0x814bef0, Scr_LoadScript);
	SetJump(0x8170a26, SV_DropClient);
	SetJump(0x8179120, SV_SendMessageToClient);
	SetJump(0x817a23e, SV_UpdateServerCommandsToClient);
	SetJump(0x816f828, SV_WriteDownloadToClient);
	SetJump(0x817a392, SV_WriteSnapshotToClient);
	SetJump(0x8178da2, SV_Netchan_TransmitNextFragment);
	SetJump(0x81d76ca, FS_GetBasepath); //Prior: GetCurrentWorkingDir
	SetJump(0x808b764, ClientScr_SetSessionTeam);
	SetJump(0x80b43c4, G_LogPrintf);
	SetJump(0x80a8068, ClientUserinfoChanged);
	SetJump(0x81aa0be, Info_SetValueForKey);
	SetJump(0x81d6fca, Sys_Milliseconds);
	SetJump(0x81a9f8a, va);

	SetJump(0x8140e9c, Sys_GetValue);
	SetJump(0x8140efe, Sys_IsMainThread);
	SetJump(0x81d6be4, Sys_EnterCriticalSection);
	SetJump(0x81d6bc8, Sys_LeaveCriticalSection);
	//ToDo build Mem_Init() on its own

	*(char*)0x8215ccc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebd = '\0'; //adds a missing linebreak

}


static qboolean Sys_PatchImage()
{

	if(!Sys_MemoryProtectWrite((void*)(IMAGE_BASE + TEXT_SECTION_OFFSET), TEXT_SECTION_LENGTH))
		return qfalse;

	if(!Sys_MemoryProtectWrite((void*)(IMAGE_BASE + RODATA_SECTION_OFFSET), RODATA_SECTION_LENGTH))
		return qfalse;


	Sys_PatchImageData( );

	if(!Sys_MemoryProtectExec((void*)(IMAGE_BASE + TEXT_SECTION_OFFSET), TEXT_SECTION_LENGTH))
		return qfalse;

	if(!Sys_MemoryProtectReadonly((void*)(IMAGE_BASE + RODATA_SECTION_OFFSET), RODATA_SECTION_LENGTH))
		return qfalse;

	return qtrue;
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
    /* No retrieving of symbols where none are :( */

    if(!Sys_PatchImage())
    {
        printf("Failed to patch module: %s\n", module);
        _exit(1);
    }
}
