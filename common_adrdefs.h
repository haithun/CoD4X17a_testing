#define com_frameTime_ADDR 0x88a61e0
#define com_errorEntered_ADDR 0x88a61f4
#define com_lastError_ADDR 0x88a6220
#define cvar_modifiedFlags_ADDR 0x1402c060
#define com_codeTimeScale_ADDR 0x88a7238

#define com_errorEntered *((qboolean*)(com_errorEntered_ADDR))
#define com_frameTime *((unsigned int*)(com_frameTime_ADDR))
#define com_lastError ((char*)com_lastError_ADDR)
#define cvar_modifiedFlags *((int*)(cvar_modifiedFlags_ADDR))
#define com_codeTimeScale *((int*)(com_codeTimeScale_ADDR))
