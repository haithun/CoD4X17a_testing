#include "q_shared.h"
#include "qcommon_io.h"
#include "xassets.h"
#include "sys_patch.h"
#include "qcommon_mem.h"

void R_Init(){

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

#define NUM_ASSETTYPES 33
#define g_poolsize_ADDR 0x82401a0
#define g_assetNames_ADDR 0x8274940
#define DB_XAssetPool_ADDR 0x8240100
#define DB_FreeXAssetHeaderHandler_ADDR 0x8240060
#define DB_DynamicCloneXAssetHandler_ADDR 0x823fe80

#define MAX_XMODELS 2000
#define MAX_GFXIMAGE 3800
#define MAX_WEAPON 196
#define MAX_FX 680

typedef enum{
        XModelPieces,
        PhysPreset,
        XAnimParts,
        XModel,	
        Material,
        TechinqueSet,		// techset
        GfxImage,		// image
        snd_alias_list_t,	// sound
        SndCurve,
        LoadedSound,
        Col_Map_sp,
        Col_Map_mp,
        Com_Map,
        Game_Map_sp,
        Game_Map_mp,
        MapEnts,
        GfxMaps,
        GfxLightDef,
        UIMaps,
        Font_s,
        MenuList,		//menufile
        menuDef_t,		//menu
        LocalizeEntry,
        WeaponDef,
        SNDDriversGlobals,
        FxEffectDef,		//fx
        FxImpactTable,		//impactfx
        AIType,
        MPType,
        Character,
        XModelAlias,
        RawFile,
        StringTable
}assets_names_t;


//Are that headers ? I'm not sure
typedef struct XModelAssetsHeader_s{
        struct XModelAssetsHeader_s*	next;
        char			data[216];
}XModelAssetsHeader_t;

typedef struct WeaponDefHeader_s{
        struct WeaponDefHeader_s*	next;
        char			data[2164];
}WeaponDefHeader_t;


void XAssets_PatchLimits(){

        void* ptr;

        int size = NUM_ASSETTYPES * sizeof(void*);

        void* *DB_XAssetPool = (void*)DB_XAssetPool_ADDR;
        ptr = &DB_XAssetPool[0];

	if(!Sys_MemoryProtectWrite(ptr, size))
	{
		_exit(1);
	}
        DB_XAssetPool[XModel] = Z_Malloc(MAX_XMODELS*DB_GetXAssetTypeSize(XModel));
        DB_XAssetPool[WeaponDef] = Z_Malloc(MAX_WEAPON*DB_GetXAssetTypeSize(WeaponDef));
        DB_XAssetPool[FxEffectDef] = Z_Malloc(MAX_FX*DB_GetXAssetTypeSize(FxEffectDef));
        DB_XAssetPool[GfxImage] = Z_Malloc(MAX_GFXIMAGE*DB_GetXAssetTypeSize(GfxImage));

	if(DB_XAssetPool[XModel] == NULL || DB_XAssetPool[WeaponDef] == NULL || DB_XAssetPool[FxEffectDef] == NULL || DB_XAssetPool[GfxImage] == NULL)
	{
		Com_Error(ERR_FATAL, "Failed to get enought memory for Assets. Can not continue\n");
		return;
	
	}

	if(!Sys_MemoryProtectReadonly(ptr, size))
	{
		_exit(1);
	}

	//Patch XAssets poolsize

        int *DB_XAssetPoolSize = (int*)g_poolsize_ADDR;

	ptr = &DB_XAssetPoolSize[0];

	if(!Sys_MemoryProtectWrite(ptr, size))
	{
		_exit(1);
	}

        DB_XAssetPoolSize[XModel] = MAX_XMODELS;
        DB_XAssetPoolSize[WeaponDef] = MAX_WEAPON;
        DB_XAssetPoolSize[FxEffectDef] = MAX_FX;
        DB_XAssetPoolSize[GfxImage] = MAX_GFXIMAGE;

	if(!Sys_MemoryProtectReadonly(ptr, size))
	{
		_exit(1);
	}

}


/*	char* *g_assetNames = (char**)g_assetNames_ADDR;
	Com_Printf("Assetname: %s\n", g_assetNames[XModelPieces]);
	Com_Printf("Assetname: %s\n", g_assetNames[PhysPreset]);
	Com_Printf("Assetname: %s\n", g_assetNames[XAnimParts]);
	Com_Printf("Assetname: %s\n", g_assetNames[XModel]);
	Com_Printf("Assetname: %s\n", g_assetNames[Material]);
	Com_Printf("Assetname: %s\n", g_assetNames[TechinqueSet]);
	Com_Printf("Assetname: %s\n", g_assetNames[GfxImage]);
	Com_Printf("Assetname: %s\n", g_assetNames[snd_alias_list_t]);
	Com_Printf("Assetname: %s\n", g_assetNames[SndCurve]);
	Com_Printf("Assetname: %s\n", g_assetNames[LoadedSound]);
	Com_Printf("Assetname: %s\n", g_assetNames[Col_Map_sp]);
	Com_Printf("Assetname: %s\n", g_assetNames[Col_Map_mp]);
	Com_Printf("Assetname: %s\n", g_assetNames[Com_Map]);
	Com_Printf("Assetname: %s\n", g_assetNames[Game_Map_sp]);
	Com_Printf("Assetname: %s\n", g_assetNames[Game_Map_mp]);
	Com_Printf("Assetname: %s\n", g_assetNames[MapEnts]);
	Com_Printf("Assetname: %s\n", g_assetNames[GfxMaps]);
	Com_Printf("Assetname: %s\n", g_assetNames[GfxLightDef]);
	Com_Printf("Assetname: %s\n", g_assetNames[UIMaps]);
	Com_Printf("Assetname: %s\n", g_assetNames[Font_s]);
	Com_Printf("Assetname: %s\n", g_assetNames[MenuList]);
	Com_Printf("Assetname: %s\n", g_assetNames[menuDef_t]);
	Com_Printf("Assetname: %s\n", g_assetNames[LocalizeEntry]);
	Com_Printf("Assetname: %s\n", g_assetNames[WeaponDef]);
	Com_Printf("Assetname: %s\n", g_assetNames[SNDDriversGlobals]);
	Com_Printf("Assetname: %s\n", g_assetNames[FxEffectDef]);
	Com_Printf("Assetname: %s\n", g_assetNames[FxImpactTable]);
	Com_Printf("Assetname: %s\n", g_assetNames[AIType]);
	Com_Printf("Assetname: %s\n", g_assetNames[MPType]);
	Com_Printf("Assetname: %s\n", g_assetNames[Character]);
	Com_Printf("Assetname: %s\n", g_assetNames[XModelAlias]);
	Com_Printf("Assetname: %s\n", g_assetNames[RawFile]);
	Com_Printf("Assetname: %s\n", g_assetNames[StringTable]);*/