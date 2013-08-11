#define TEXT_SECTION_OFFSET 0x2c20
#define TEXT_SECTION_LENGTH 0x1bf1a4
#define RODATA_SECTION_OFFSET 0x1c1e00
#define RODATA_SECTION_LENGTH 0x36898
#define DATA_SECTION_OFFSET 0x222580
#define DATA_SECTION_OFFSET_FIX 0x1000
#define DATA_SECTION_LENGTH 0x9454
#define IMAGE_BASE 0x8048000

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