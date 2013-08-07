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
} cvar_t;
