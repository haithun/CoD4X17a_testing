//typedef int32_t DWORD;


#include "sys_patch.h"

void Sys_RetriveData(){


#define startADR 0x817b541
#define endADR 0x817b56b

    FILE * fdout;
    byte* adr;
    int i;

    adr = (byte*)(startADR);

    char buf[32];

    fdout=fopen("diffout.bin", "w");
    if(fdout){

        fwrite("static byte patchblock_??[] = {", 1, 31 ,fdout);

        Com_sprintf(buf, sizeof(buf), " 0x%X, 0x%X, 0x%X, 0x%X, ", (int)adr & 0xff, ((int)adr >> 8) & 0xff, ((int)adr >> 16) & 0xff, ((int)adr >> 24) & 0xff);

        fwrite(buf, 1, strlen(buf),fdout);

        for(i = 0; i < endADR - startADR +1; i++, adr++)
        {
            if(i % 16 == 0)
            {
                fwrite("\n\t", 1, 2 ,fdout);
            }

            Com_sprintf(buf, sizeof(buf), "0x%X, ", *adr);
            fwrite(buf, 1, strlen(buf) ,fdout);
        }

        fwrite("\n};\n", 1, 3 ,fdout);
        fclose(fdout);
    }

    _exit(0);
}



void Sys_PatchImageWithBlock(byte *block, int blocksize)
{

    int startadr;
    byte* startadrasbytes = (byte*)&startadr;

    startadrasbytes[0] = block[0];
    startadrasbytes[1] = block[1];
    startadrasbytes[2] = block[2];
    startadrasbytes[3] = block[3];

//    printf("Block Start address is: %X\n", startadr);

    memcpy((void*)startadr, &block[4], blocksize - 4);

}

void Sys_PatchImageData( void *ptr )
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

	SetCallFSO(0x8ab1, Jump_CalcHeight);
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
	SetJump(0x8111120, Cmd_List_f);
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

	//ToDo build Mem_Init() on its own
/*
	*(char*)0x8215ccc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebc = '\n'; //adds a missing linebreak
	*(char*)0x8222ebd = '\0'; //adds a missing linebreak
*/
}


qboolean Sys_PatchImage()
{

        void* ptr;
        int size;

        ptr = (void*)(IMAGE_BASE + TEXT_SECTION_OFFSET);

	size = TEXT_SECTION_LENGTH;

	if(!Sys_MemoryProtectWrite(ptr, size))
		return qfalse;

	Sys_PatchImageData( ptr );

	if(!Sys_MemoryProtectExec(ptr, size))
		return qfalse;

	return qtrue;
}


DWORD SetCall(DWORD addr, void* destination){

	DWORD callwidth;
	DWORD restore;
	byte* baddr = (byte*)addr;

	callwidth = (DWORD)( destination - (void*)baddr - 5);
	*baddr = 0xe8;
	baddr++;

	restore = *(DWORD*)baddr;
	*(DWORD*)baddr = callwidth;

	return restore;
}

DWORD SetCallFSO(DWORD addr, void* destination){

    return SetCall( addr + IMAGE_BASE, destination);

}


DWORD SetJump(DWORD addr, void* destination){

	DWORD jmpwidth;
	DWORD restore;
	byte* baddr = (byte*)addr;

	jmpwidth = (DWORD)( destination - (void*)baddr - 5);
	*baddr = 0xe9;
	baddr++;

	restore = *(DWORD*)baddr;
	*(DWORD*)baddr = jmpwidth;

	return restore;
}

DWORD SetJumpFSO(DWORD addr, void* destination){

    return SetJump( addr + IMAGE_BASE, destination);

}