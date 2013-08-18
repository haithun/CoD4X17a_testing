#include "misc.h"

char* SL_ConvertToString(unsigned int index)
{

    char** ptr = (char**)STRBUFFBASEPTR_ADDR;
    char* base = *ptr;
    return &base[ index*12 + 4];
}
