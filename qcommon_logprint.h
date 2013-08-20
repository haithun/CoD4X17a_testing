#ifndef __QCOMMON_LOGPRINT_H__
#define __QCOMMON_LOGPRINT_H__

#include "q_shared.h"

void QDECL SV_EnterLeaveLog( const char *fmt, ... );
void QDECL Com_PrintAdministrativeLog( const char *msg );
void Com_PrintLogfile( const char *msg );
void Com_CloseLogFiles(void);

#endif

