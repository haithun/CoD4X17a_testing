#include "misc.h"
#include "qcommon_io.h"
#include "g_sv_shared.h"
#include "punkbuster.h"
#include "hl2rcon.h"

char* SL_ConvertToString(unsigned int index)
{

    char** ptr = (char**)STRBUFFBASEPTR_ADDR;
    char* base = *ptr;
    return &base[ index*12 + 4];
}


void AddRedirectLocations()
{
    Com_AddRedirect(PbCapatureConsoleOutput_wrapper);

}