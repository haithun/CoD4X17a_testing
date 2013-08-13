SECTION .text

global DB_SetInitializing
DB_SetInitializing:
    jmp 0x820337c

global DB_FileExists
DB_FileExists:
    jmp 0x8204424

global DB_ModFileExists
DB_ModFileExists:
    jmp 0x8204470

global DB_LoadXAssets
DB_LoadXAssets:
    jmp 0x8205e86

global DB_GetXAssetTypeSize
DB_GetXAssetTypeSize:
    jmp 0x81da6ce

global XAnimInit
XAnimInit:
    jmp 0x81b649c
