#ifndef __SYS_COD4DEFS_H__
#define __SYS_COD4DEFS_H__


#define COD4_DLL "cod4_lnxded.so"
#define COD4_BINNAME "cod4_lnxded"

#define TEXT_SECTION_OFFSET 0x2c20
#define TEXT_SECTION_LENGTH 0x1bf1a4
#define RODATA_SECTION_OFFSET 0x1c1e00
#define RODATA_SECTION_LENGTH 0x36898
#define DATA_SECTION_OFFSET 0x222580
#define DATA_SECTION_OFFSET_FIX 0x1000
#define DATA_SECTION_LENGTH 0x9454
#define IMAGE_BASE 0x8048000




#define Q3_VERSION "1.7a"
#define GAME_STRING "CoD4 X"

#include "version.h"
#ifndef BUILD_NUMBER
#define BUILD_NUMBER -1
#endif

#endif