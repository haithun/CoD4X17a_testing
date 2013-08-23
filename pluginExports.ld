{
  extern "C"
  {
    Plugin_*;
    
    
    Com_Printf;
    Com_PrtinWarning;
    Com_PrintError;
    Com_DPrintf;
    Com_ParseGetToken;
    Com_ParseTokenLength;
    
    Cmd_Argv;
    Cmd_Argc;
    
    Cvar_VariableStringBuffer;
    Cvar_VariableValue;
    Cvar_VariableIntegerValue;
    Cvar_VariableString;
    Cvar_RegisterString;
    Cvar_RegisterBool;
    Cvar_RegisterInt;
    Cvar_RegisterEnum;
    Cvar_RegisterFloat;
    Cvar_SetInt;
    Cvar_SetBool;
    Cvar_SetString;
    Cvar_SetFloat;
    
    FS_SV_FOpenFileRead;
    FS_SV_FOpenFileWrite;
    FS_Read;
    FS_ReadLine;
    FS_Write;
    FS_FCloseFile;
    
    NET_StringToAdr;
    
    Sys_Milliseconds;
  };
};

