SECTION .text

global SL_Init
SL_Init:
    jmp 0x8150928

global Swap_Init
Swap_Init:
    jmp 0x81aa7b6

global CSS_InitConstantConfigStrings
CSS_InitConstantConfigStrings:
    jmp 0x8185a72

global Con_InitChannels
Con_InitChannels:
    jmp 0x82096be

global SEH_UpdateLanguageInfo
SEH_UpdateLanguageInfo:
    jmp 0x8180432

global SetAnimCheck
SetAnimCheck:
    jmp 0x81423f0

global BG_IsWeaponValid
BG_IsWeaponValid:
    jmp 0x805f4fe

global SEH_StringEd_GetString
SEH_StringEd_GetString:
    jmp 0x817fbe0

global DObjInit
DObjInit:
    jmp 0x81acb00

global SL_RemoveRefToString
SL_RemoveRefToString:
    jmp 0x8150e24