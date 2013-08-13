#include "q_shared.h"

void Sys_PatchImageWithBlock(byte *block, int blocksize);
void Sys_PatchImageData( void );
qboolean Sys_PatchImage();
DWORD SetCall(DWORD addr, void* destination);
DWORD SetCallFSO(DWORD addr, void* destination);
DWORD SetJump(DWORD addr, void* destination);
DWORD SetJumpFSO(DWORD addr, void* destination);

qboolean Sys_MemoryProtectWrite(void* startoffset, int len);
qboolean Sys_MemoryProtectExec(void* startoffset, int len);
qboolean Sys_MemoryProtectReadonly(void* startoffset, int len);