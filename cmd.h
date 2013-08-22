#ifndef __CMD_H__
#define __CMD_H__

#include "q_shared.h"

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;

#define MAX_CMD_BUFFER  16384
#define MAX_CMD_LINE    1024
/*
typedef struct {
	byte    *data;
	int maxsize;
	int cursize;
} cmd_t;

int cmd_wait;
cmd_t cmd_text;
byte cmd_text_buf[MAX_CMD_BUFFER];
*/

typedef void *xcommand_t;
typedef void (*completionFunc_t)( char *args, int argNum );

typedef struct cmd_function_s
{
	struct cmd_function_s   *next;
	char                    *name;
	int			minPower;
	completionFunc_t	complete;
	xcommand_t function;
} cmd_function_t;


//static int cmd_argc;
//static char        *cmd_argv[MAX_STRING_TOKENS];        // points into cmd_tokenized
//static char cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];         // will have 0 bytes inserted
//static char cmd_cmd[BIG_INFO_STRING];         // the original command we received (no token processing)

#define MAX_POWERLIST 256


void __cdecl Cmd_Init(void);
void __cdecl Cmd_TokenizeString(const char* string);
void __cdecl SV_Cmd_TokenizeString(const char* string);
void __cdecl Cmd_EndTokenizeString();
void __cdecl SV_Cmd_EndTokenizeString();
void __cdecl Cmd_ExecuteSingleCommand(int unk, int unk2, const char *cmd );
void __cdecl Cbuf_Init(void);
void __cdecl Cbuf_Execute(int, int);
void __cdecl Cbuf_ExecuteBuffer(int, int, char* buf);
void __cdecl Cbuf_AddText(int dummy, const char* text);

qboolean Cmd_AddCommand( const char *cmd_name, xcommand_t function );
qboolean Cmd_RemoveCommand( const char *cmd_name );
qboolean Cmd_SetPower(const char *cmd_name, int power);
int Cmd_GetPower(const char *cmd_name);
void	Cmd_ResetPower();

int	SV_Cmd_Argc( void );
int	Cmd_Argc( void );
char	*SV_Cmd_Argv( int arg );
void	SV_Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
char	*Cmd_Argv( int arg );
char	*Cmd_Args( char* buff, int bufsize );
char	*Cmd_Argsv( int arg, char* buff, int bufsize );
void	Cmd_CommandCompletion( void(*callback)(const char *s) );
void	Cmd_CompleteArgument( const char *command, char *args, int argNum );
void	Cmd_SetCommandCompletionFunc( const char *command, completionFunc_t complete );
void	Cmd_AddCommands(void);
void	Cmd_WritePowerConfig(char* buffer, int size);
qboolean Cmd_InfoSetPower( const char *infostring );

void	Cmd_Vstr_f(void);
void	Cmd_Wait_f(void);

#endif

