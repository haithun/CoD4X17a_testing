#ifndef __SYS_CON_TTY_H__
#define __SYS_CON_TTY_H__

void Sys_AnsiColorPrint( const char *msg );
void CON_Shutdown( void );
void CON_Init(void);
char *CON_Input( void );
void CON_Print( const char *msg );

#endif
