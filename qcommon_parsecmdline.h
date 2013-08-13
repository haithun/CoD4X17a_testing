#ifndef __COMMON_PARSECMDLINE_H__
#define __COMMON_PARSECMDLINE_H__

#include "q_shared.h"

void Com_StartupVariable( const char *match );
qboolean Com_AddStartupCommands( void );
void Com_ParseCommandLine( char *commandLine );

#endif