#define QDECL
#define PLUGIN_COMPILE
#include <string.h>


	//Types and structs
typedef int	fileHandle_t;
typedef enum {qfalse, qtrue}	qboolean;
typedef void *xcommand_t;
typedef void *client_t_ptr;
typedef unsigned char byte;

// Used for internet communication
typedef enum {
	NA_BAD = 0,					// an address lookup failed
	NA_BOT = 0,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IP6 = 5,
	NA_TCP = 6,
	NA_TCP6 = 7,
	NA_MULTICAST6 = 8,
	NA_UNSPEC = 9,
	NA_DOWN = 10
} netadrtype_t;

typedef struct {
	byte	type;
	byte	scope_id;
	unsigned short	port;
	short sock;
        union{
	    byte	ip[4];
	    byte	ipx[10];
	    byte	ip6[16];
	};
}netadr_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef void convariable_t;

typedef struct{		// A structure representing a player's scoreboard
    int	score;
    int	deaths;
    int	kills;
    int	assists;
}clientScoreboard_t;


#include "plugin_declarations.h"	// Function descriptions are available in this file as well
#include "function_declarations.h"

typedef enum{
    CVAR_BOOL,
    CVAR_FLOAT,
    CVAR_VEC2,
    CVAR_VEC3,
    CVAR_VEC4,
    CVAR_INT,
    CVAR_ENUM,
    CVAR_STRING,
    CVAR_COLOR
}cvarType_t;

typedef struct{
	char *name;
	char *description;
	short int flags;
	byte type;
	byte modified;
	union{
		float floatval;
		float value;
		int integer;
		char* string;
		byte boolean;
	};
	union{
		float latchedfloatval;
		float latchedvalue;
		int latchedinteger;
		char* latchedstring;
		byte latchedboolean;
	};
} cvar_t;
#define cvardeclarations