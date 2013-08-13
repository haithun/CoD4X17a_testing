#ifndef __XASSETS_H__
#define __XASSETS_H__

#include "q_shared.h"

typedef struct{
    const char*		fastfile;
    int			loadpriority;
    int			notknown;
}XZoneInfo;

void R_Init();
void __cdecl DB_SetInitializing(qboolean);
byte __cdecl DB_FileExists(const char* filename, int mode);
qboolean __cdecl DB_ModFileExists(void);
void __cdecl DB_LoadXAssets(XZoneInfo*, unsigned int assetscount, int);
int __cdecl DB_GetXAssetTypeSize(int type);
void __cdecl XAnimInit(void);
void XAssets_PatchLimits(void);

#endif
