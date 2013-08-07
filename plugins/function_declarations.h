#ifndef __cdecl
#define __cdecl __attribute__((cdecl))
#endif

#ifdef PLUGIN_COMPILE

#define CONVAR_T convariable_t

#else
#define CONVAR_T cvar_t

#endif

	//	Stock CoD4 functions
	void (QDECL *Com_Printf)( const char *fmt, ...);	// Print to a correct place (rcon, player console, logs)
	void (QDECL *Com_PrintWarning)( const char *fmt, ...);	// Print to a correct place (rcon, player console, logs)
	void (QDECL *Com_PrintError)( const char *fmt, ...);	// Print to a correct place (rcon, player console, logs)
	void (QDECL *Com_DPrintf)( const char *fmt, ...);	// Same as Com_Printf, only shows up when developer is set to 1
	__cdecl char* (*Cmd_Argv)(int arg);				// Get a command argument with index arg.
	__cdecl int (*Cmd_Argc)();					// Get number of command arguments
	__cdecl int (*Sys_Milliseconds)();
	//	File handling functions
	__cdecl int (*FS_SV_FOpenFileRead)(const char *filename, fileHandle_t *fp);	// Open a file for reading
	__cdecl fileHandle_t (*FS_SV_FOpenFileWrite)(const char *filename);		// Open a file for writing
	__cdecl int (*FS_Read)(void *buffer, int len, fileHandle_t f);			// Read data from file
	__cdecl int (*FS_ReadLine)(void *buffer, int len, fileHandle_t f);		// Read a line from file
	__cdecl int (*FS_Write)(const void *buffer, int len, fileHandle_t h);		// Write to file
	__cdecl qboolean (*FS_FCloseFile)(fileHandle_t f);				// Cloase an open file
	//	String functions
	__cdecl char* (*Com_ParseGetToken)(char* line);					// Tokenize a string - get next token
	__cdecl int (*Com_ParseTokenLength)(char* token);				// Tokenize a string - get the token's length
	//	Cvar value retriving functions
	__cdecl void (*Cvar_VariableStringBuffer)(const char* cvarname, char* buff, size_t size);
	__cdecl float (*Cvar_VariableValue)( const char *var_name );
	__cdecl int (*Cvar_VariableIntegerValue)( const char *var_name );
	__cdecl const char* (*Cvar_VariableString)( const char *var_name );
	//	Custom functions
	__cdecl clientScoreboard_t (*Plugin_GetClientScoreboard)(int clientNum);	// Get a scoreboard of a player
	__cdecl int (*Plugin_Cmd_GetInvokerUid)();					// Get UID of command invoker
	__cdecl int (*Plugin_GetPlayerUid)(int slot);					// Get UID of a plyer
	__cdecl int (*Plugin_GetSlotCount)();						// Get number of server slots
	__cdecl qboolean (*Plugin_IsSvRunning)();					// Get number of server slots
	void (QDECL *Plugin_ChatPrintf)(int slot, char *fmt, ...);			// Print to chat to a player (-1 for all)
	void (QDECL *Plugin_BoldPrintf)(int slot, char *fmt, ...);			// Print to the player's screen (-1 for all)
	__cdecl char *(*Plugin_GetPlayerName)(int slot);				// Get a name of a player
	__cdecl void (*Plugin_AddCommand)(char *name, xcommand_t command, int defaultpower);		// Add a server command
	__cdecl void *(*Plugin_Malloc)(size_t size);					// Same as standard C function malloc
	__cdecl void (*Plugin_Free)(void *ptr);						// Same as standard C function free
	__cdecl void (*Plugin_Error)(int code, char *string);				// Notify the server of an error, action depends on code parameter
	__cdecl int (*Plugin_GetLevelTime)();
	__cdecl int (*Plugin_GetServerTime)();
	//	TCP Connection functions
	__cdecl qboolean (*Plugin_TcpConnect)(int connection, const char* remote);	// Open a new connection
	__cdecl int (*Plugin_TcpGetData)(int connection, void *buf, int size);	// Recieve data
	__cdecl qboolean (*Plugin_TcpSendData)(int connection, void *data, int len);	// Send data
	__cdecl void (*Plugin_TcpCloseConnection)(int connection);			// Close an open connection
	__cdecl qboolean (*Plugin_UdpSendData)(netadr_t* to, void* data, int len);
	__cdecl void (*Plugin_ServerPacketEvent)(netadr_t* to, void* data, int len);
	__cdecl int (*NET_StringToAdr)(const char* string, netadr_t* , netadrtype_t);
	//	Cvars
	CONVAR_T* (__cdecl *Cvar_RegisterString)(const char *var_name, const char *var_value, int flags, const char *var_description);
	CONVAR_T* (__cdecl *Cvar_RegisterBool)(const char *var_name, qboolean var_value, int flags, const char *var_description);
	CONVAR_T* (__cdecl *Cvar_RegisterInt)(const char *var_name, int var_value, int min_value, int max_value, int flags, const char *var_description);
	CONVAR_T* (__cdecl *Cvar_RegisterEnum)(const char *var_name, char** valnames, int defaultval, int flags, const char *var_description);
	CONVAR_T* (__cdecl *Cvar_RegisterFloat)(const char *var_name, float var_value, float min_value, float max_value, int flags, const char *var_description);
	void (__cdecl *Cvar_SetInt)(CONVAR_T const* var, int val);
	void (__cdecl *Cvar_SetBool)(CONVAR_T const* var, qboolean val);
	void (__cdecl *Cvar_SetString)(CONVAR_T const* var, char const* string);
	void (__cdecl *Cvar_SetFloat)(CONVAR_T const* var, float val);
	//UIDS / GUIDs
	void (__cdecl *Plugin_SetPlayerUID)(unsigned int clientslot, unsigned int uid);
	unsigned int (__cdecl *Plugin_GetPlayerUID)(unsigned int clientslot);
	const char* (__cdecl *Plugin_GetPlayerGUID)(unsigned int clientslot);
	void (__cdecl *Plugin_SetPlayerGUID)(unsigned int clientslot, const char* guid);
	void (__cdecl *Plugin_SetPlayerNoPB)(unsigned int clientslot);
	int (__cdecl *Plugin_DoesServerUseUids)(void);
	void (__cdecl *Plugin_SetServerToUseUids)(int useuids);