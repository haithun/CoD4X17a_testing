#define SCRSTRUCT_ADDR 0x895bf08
#define STRINGINDEX_ADDR 0x836fe20
#define stringIndex (*((stringIndex_t*)(STRINGINDEX_ADDR)))

typedef struct{
	short   emptystring;
	short   active;
	short   j_spine4;
	short   j_helmet;
	short   j_head;
	short   all;
	short   allies;
	short   axis;
	short   bad_path;
	short   begin_firing;
	short   cancel_location;
	short   confirm_location;
	short   crouch;
	short   current;
	short   damage;
	short   dead;
	short   death;
	short   detonate;
	short   direct;
	short   dlight;
	short   done;
	short   empty;
	short   end_firing;
	short   entity;
	short   explode;
	short   failed;
	short   fraction;
	short   free;
	short   goal;
	short   goal_changed;
	short   goal_yaw;
	short   grenade;
	short   grenade_danger;
	short   grenade_fire;
	short   grenade_pullback;
	short   info_notnull;
	short   invisible;
	short   key1;
	short   key2;
	short   killanimscript;
	short   left;
	short   light;
	short   movedone;
	short   noclass;
	short   none;
	short   normal;
	short   player;
	short   position;
	short   projectile_impact;
	short   prone;
	short   right;
	short   reload;
	short   reload_start;
	short   rocket;
	short   rotatedone;
	short   script_brushmodel;
	short   script_model;
	short   script_origin;
	short   snd_enveffectsprio_level;
	short   snd_enveffectsprio_shellshock;
	short   snd_channelvolprio_holdbreath;
	short   snd_channelvolprio_pain;
	short   snd_channelvolprio_shellshock;
	short   stand;
	short   suppression;
	short   suppression_end;
	short   surfacetype;
	short   tag_aim;
	short   tag_aim_animated;
	short   tag_brass;
	short   tag_butt;
	short   tag_clip;
	short   tag_flash;
	short   tag_flash_11;
	short   tag_flash_2;
	short   tag_flash_22;
	short   tag_flash_3;
	short   tag_fx;
	short   tag_inhand;
	short   tag_knife_attach;
	short   tag_knife_fx;
	short   tag_laser;
	short   tag_origin;
	short   tag_weapon;
	short   tag_player;
	short   tag_camera;
	short   tag_weapon_right;
	short   tag_gasmask;
	short   tag_gasmask2;
	short   tag_sync;
	short   target_script_trigger;
	short   tempEntity;
	short   top;
	short   touch;
	short   trigger;
	short   trigger_use;
	short   trigger_use_touch;
	short   trigger_damage;
	short   trigger_lookat;
	short   truck_cam;
	short   weapon_change;
	short   weapon_fired;
	short   worldspawn;
	short   flashbang;
	short   flash;
	short   smoke;
	short   night_vision_on;
	short   night_vision_off;
	short   MOD_UNKNOWN;
	short   MOD_PISTOL_BULLET;
	short   MOD_RIFLE_BULLET;
	short   MOD_GRENADE;
	short   MOD_GRENADE_SPLASH;
	short   MOD_PROJECTILE;
	short   MOD_PROJECTILE_SPLASH;
	short   MOD_MELEE;
	short   MOD_HEAD_SHOT;
	short   MOD_CRUSH;
	short   MOD_TELEFRAG;
	short   MOD_FALLING;
	short   MOD_SUICIDE;
	short   MOD_TRIGGER_HURT;
	short   MOD_EXPLOSIVE;
	short   MOD_IMPACT;
	short   script_vehicle;
	short   script_vehicle_collision;
	short   script_vehicle_collmap;
	short   script_vehicle_corpse;
	short   turret_fire;
	short   turret_on_target;
	short   turret_not_on_target;
	short   turret_on_vistarget;
	short   turret_no_vis;
	short   turret_rotate_stopped;
	short   turret_deactivate;
	short   turretstatechange;
	short   turretownerchange;
	short   reached_end_node;
	short   reached_wait_node;
	short   reached_wait_speed;
	short   near_goal;
	short   veh_collision;
	short   veh_predictedcollision;
	short   auto_change;
	short   back_low;
	short   back_mid;
	short   back_up;
	short   begin;
	short   call_vote;
	short   freelook;
	short   head;
	short   intermission;
	short   j_head_dup;
	short   manual_change;
	short   menuresponse;
	short   neck;
	short   pelvis;
	short   pistol;
	short   plane_waypoint;
	short   playing;
	short   spectator;
	short   vote;
	short   sprint_begin;
	short   sprint_end;
	short   tag_driver;
	short   tag_passenger;
	short   tag_gunner;
	short   tag_wheel_front_left;
	short   tag_wheel_front_right;
	short   tag_wheel_back_left;
	short   tag_wheel_back_right;
	short   tag_wheel_middle_left;
	short   tag_wheel_middle_right;
	short   script_vehicle_collision_dup;
	short   script_vehicle_collmap_dup;
	short   script_vehicle_corpse_dup;
	short   tag_detach;
	short   tag_popout;
	short   tag_body;
	short   tag_turret;
	short   tag_turret_base;
	short   tag_barrel;
	short   tag_engine_left;
	short   tag_engine_right;
	short   front_left;
	short   front_right;
	short   back_left;
	short   back_right;
	short   tag_gunner_pov;
}stringIndex_t;


//typedef void *xcommand_t;

typedef struct scr_function_s
{
	struct scr_function_s	*next;
	char		*name;
	xcommand_t		function;
	qboolean		developer;
} scr_function_t;



typedef byte PrecacheEntry[8192];
typedef unsigned int sval_u;
typedef int scr_entref_t;

/**************** Additional *************************/

typedef enum{
    SCR_CB_NEW_SAY,
    SCR_CB_NEW_SEQMSG,
    SCR_CB_NEW_SEQPLAYERMSG
}script_CallBacks_new_t;

int script_CallBacks_new[8];
qboolean say_forwardAll;


typedef struct{
	char lastCommand[32];
	int lastTicket;
	char recvData[MAX_TOKEN_CHARS];
	char sendData[MAX_TOKEN_CHARS];
}scrMasterCommBuff_t;

scrMasterCommBuff_t scrCommBuff;

typedef int fieldtype_t;

typedef struct
{
    char* name;
    int val1;
    fieldtype_t type;
    void (*setfun)();
    void (*getfun)();

}client_fields_t;




void QDECL Scr_PrintScriptRuntimeWarning(const char* fmt,...);


typedef void (__cdecl *tScr_InitVariables)(void);			//VM
tScr_InitVariables Scr_InitVariables = (tScr_InitVariables)(0x815288a);

typedef void (__cdecl *tScr_Init)(void);			//VM_Init
tScr_Init Scr_Init = (tScr_Init)(0x815d8e2);

typedef void (__cdecl *tScr_Settings)(int, int, int);
tScr_Settings Scr_Settings = (tScr_Settings)(0x815cf90);

typedef void (__cdecl *tScr_AddEntity)(gentity_t* ent);
tScr_AddEntity Scr_AddEntity = (tScr_AddEntity)(0x80c7770);

typedef void (__cdecl *tScr_Cleanup)(void);
tScr_Cleanup Scr_Cleanup = (tScr_Cleanup)(0x815cf84);

typedef void (__cdecl *tGScr_Shutdown)(void);
tGScr_Shutdown GScr_Shutdown = (tGScr_Shutdown)(0x80bf610);

typedef short (__cdecl *tScr_AllocArray)();
tScr_AllocArray Scr_AllocArray = (tScr_AllocArray)(0x8153cca);

typedef int (__cdecl *tScr_GetNumParam)( void );
tScr_GetNumParam Scr_GetNumParam = (tScr_GetNumParam)(0x815d01e);

typedef int (__cdecl *tScr_GetInt)( unsigned int );
tScr_GetInt Scr_GetInt = (tScr_GetInt)(0x8160fee);

typedef float (__cdecl *tScr_GetFloat)( unsigned int );
tScr_GetFloat Scr_GetFloat = (tScr_GetFloat)(0x816094c);

typedef char* (__cdecl *tScr_GetString)( unsigned int );
tScr_GetString Scr_GetString = (tScr_GetString)(0x8160932);

typedef gentity_t* (__cdecl *tScr_GetEntity)( unsigned int );
tScr_GetEntity Scr_GetEntity = (tScr_GetEntity)(0x80c76ec);

typedef short (__cdecl *tScr_GetConstString)( unsigned int );
tScr_GetConstString Scr_GetConstString = (tScr_GetConstString)(0x816074c);

typedef unsigned int (__cdecl *tScr_GetType)( unsigned int );
tScr_GetType Scr_GetType = (tScr_GetType)(0x815f7c8);

typedef void (__cdecl *tScr_GetVector)( unsigned int, vec3_t* );
tScr_GetVector Scr_GetVector = (tScr_GetVector)(0x815ffe6);

typedef void (__cdecl *tScr_Error)( const char *string);
tScr_Error Scr_Error = (tScr_Error)(0x815e9f4);

typedef void (__cdecl *tScr_SetLoading)( qboolean );
tScr_SetLoading Scr_SetLoading = (tScr_SetLoading)(0x815cfba);

typedef void (__cdecl *tScr_ParamError)( int, const char *string);
tScr_ParamError Scr_ParamError = (tScr_ParamError)(0x815f20e);

typedef void (__cdecl *tScr_ObjectError)( const char *string);
tScr_ObjectError Scr_ObjectError = (tScr_ObjectError)(0x815f134);

typedef void (__cdecl *tScr_AddInt)(int value);
tScr_AddInt Scr_AddInt = (tScr_AddInt)(0x815f01a);

typedef void (__cdecl *tScr_AddFloat)(float);
tScr_AddFloat Scr_AddFloat = (tScr_AddFloat)(0x815ef9a);

typedef void (__cdecl *tScr_AddBool)(qboolean);
tScr_AddBool Scr_AddBool = (tScr_AddBool)(0x815eac6);

typedef void (__cdecl *tScr_AddString)(const char *string);
tScr_AddString Scr_AddString = (tScr_AddString)(0x815ec68);

typedef void (__cdecl *tScr_AddUndefined)(void);
tScr_AddUndefined Scr_AddUndefined = (tScr_AddUndefined)(0x815eea2);

typedef void (__cdecl *tScr_AddVector)( vec3_t vec );
tScr_AddVector Scr_AddVector = (tScr_AddVector)(0x815ee12);

typedef void (__cdecl *tScr_AddArray)( void );
tScr_AddArray Scr_AddArray = (tScr_AddArray)(0x815d5c0);

typedef void (__cdecl *tScr_MakeArray)( void );
tScr_MakeArray Scr_MakeArray = (tScr_MakeArray)(0x815ed8a);

typedef void (__cdecl *tScr_Notify)( gentity_t*, unsigned short, unsigned int);
tScr_Notify Scr_Notify = (tScr_Notify)(0x80c7604);

typedef void (__cdecl *tScr_NotifyNum)( int, unsigned int, unsigned int, unsigned int);
tScr_NotifyNum Scr_NotifyNum = (tScr_NotifyNum)(0x815e762);

/*Not working :(
typedef void (__cdecl *tScr_PrintPrevCodePos)( int printDest, const char* unk, qboolean unk2 );
tScr_PrintPrevCodePos Scr_PrintPrevCodePos = (tScr_PrintPrevCodePos)(0x814ef6e);
*/

typedef int (__cdecl *tScr_GetFunctionHandle)( const char* scriptName, const char* labelName);
tScr_GetFunctionHandle Scr_GetFunctionHandle = (tScr_GetFunctionHandle)(0x814c1b4);

typedef short (__cdecl *tScr_ExecEntThread)( gentity_t* ent, int callbackHook, unsigned int numArgs);
tScr_ExecEntThread Scr_ExecEntThread = (tScr_ExecEntThread)(0x80c765c);

typedef short (__cdecl *tScr_ExecThread)( int callbackHook, unsigned int numArgs);
tScr_ExecThread Scr_ExecThread = (tScr_ExecThread)(0x8165032);

typedef void (__cdecl *tScr_FreeThread)( short threadId);
tScr_FreeThread Scr_FreeThread = (tScr_FreeThread)(0x815d062);

typedef unsigned int (__cdecl *tScr_CreateCanonicalFilename)( const char* name );
tScr_CreateCanonicalFilename Scr_CreateCanonicalFilename = (tScr_CreateCanonicalFilename)(0x81516ee);
//Unknown real returntype
typedef unsigned int (__cdecl *tFindVariable)( unsigned int, unsigned int );
tFindVariable FindVariable = (tFindVariable)(0x81542d4);

typedef void (__cdecl *tSL_RemoveRefToString)( unsigned int );
tSL_RemoveRefToString SL_RemoveRefToString = (tSL_RemoveRefToString)(0x8150e24);

typedef unsigned int (__cdecl *tFindObject)( unsigned int );
tFindObject FindObject = (tFindObject)(0x8152294);

typedef unsigned int (__cdecl *tGetNewVariable)( unsigned int, unsigned int );
tGetNewVariable GetNewVariable = (tGetNewVariable)(0x81545ce);

//typedef const char* (__cdecl *tSL_ConvertToString)( unsigned int );
//tSL_ConvertToString SL_ConvertToString = (tSL_ConvertToString)(0x8150340);

typedef void * (__cdecl *tTempMalloc)( int );
tTempMalloc TempMalloc = (tTempMalloc)(0x8151dce);

typedef void (__cdecl *tScriptParse)( sval_u* , byte);
tScriptParse ScriptParse = (tScriptParse)(0x816b5da);

typedef unsigned int (__cdecl *tGetObjectA)( unsigned int );
tGetObjectA GetObjectA = (tGetObjectA)(0x8154046);

typedef unsigned int (__cdecl *tGetVariable)( unsigned int, unsigned int );
tGetVariable GetVariable = (tGetVariable)(0x815540a);

typedef void (__cdecl *tScriptCompile)( sval_u, unsigned int, unsigned int, PrecacheEntry*, int);
tScriptCompile ScriptCompile = (tScriptCompile)(0x81491d8);

typedef void* (__cdecl *tScr_AddSourceBuffer)( const char*, const char*, const char*, byte );
tScr_AddSourceBuffer Scr_AddSourceBuffer = (tScr_AddSourceBuffer)(0x814fbac);

typedef void (__cdecl *tScr_InitAllocNode)( void );
tScr_InitAllocNode Scr_InitAllocNode = (tScr_InitAllocNode)(0x814fea6);

typedef void (__cdecl *tScr_BeginLoadScripts)( void );
tScr_BeginLoadScripts Scr_BeginLoadScripts = (tScr_BeginLoadScripts)(0x814c266);

typedef void (__cdecl *tScr_SetClassMap)( unsigned int );
tScr_SetClassMap Scr_SetClassMap = (tScr_SetClassMap)(0x8153a3a);

typedef void (__cdecl *tScr_AddFields)( unsigned int, const char*, unsigned int );
tScr_AddFields Scr_AddFields = (tScr_AddFields)(0x81535ba);

typedef void (__cdecl *tScr_SetGenericField)( void*, fieldtype_t, int );
tScr_SetGenericField Scr_SetGenericField = (tScr_SetGenericField)(0x80c7eae);

typedef void (__cdecl *tScr_GetGenericField)( void*, fieldtype_t, int );
tScr_GetGenericField Scr_GetGenericField = (tScr_GetGenericField)(0x80c7d36);

/*
typedef void (__cdecl *tGScr_AddFieldsForEntity)( void );
tGScr_AddFieldsForEntity GScr_AddFieldsForEntity = (tGScr_AddFieldsForEntity)(0x80c7808);
*/
typedef void (__cdecl *tGScr_AddFieldsForHudElems)( void );
tGScr_AddFieldsForHudElems GScr_AddFieldsForHudElems = (tGScr_AddFieldsForHudElems)(0x808db80);

typedef void (__cdecl *tGScr_AddFieldsForRadiant)( void );
tGScr_AddFieldsForRadiant GScr_AddFieldsForRadiant = (tGScr_AddFieldsForRadiant)(0x80c77ec);

typedef void (__cdecl *tScr_AddHudElem)( game_hudelem_t* );
tScr_AddHudElem Scr_AddHudElem = (tScr_AddHudElem)(0x80c7746);

typedef void (__cdecl *tScr_FreeHudElem)( game_hudelem_t* );
tScr_FreeHudElem Scr_FreeHudElem = (tScr_FreeHudElem)(0x80c778e);

typedef void (__cdecl *tScr_EndLoadScripts)( void );
tScr_EndLoadScripts Scr_EndLoadScripts = (tScr_EndLoadScripts)(0x814bcee);

/*
typedef void (__cdecl *tHECmd_SetText)( scr_entref_t );
tHECmd_SetText HECmd_SetText = (tHECmd_SetText)(0x808f7f6);
*/

typedef void (__cdecl *tScr_ConstructMessageString)( int, int, const char*, char*, unsigned int );
tScr_ConstructMessageString Scr_ConstructMessageString = (tScr_ConstructMessageString)(0x80c02aa);

typedef int (__cdecl *tG_LocalizedStringIndex)( const char* );
tG_LocalizedStringIndex G_LocalizedStringIndex = (tG_LocalizedStringIndex)(0x80cb218);

qboolean Scr_PlayerSay(gentity_t*, int team, const char* text);

void GScr_LoadGameTypeScript(void);

unsigned int Scr_LoadScript(const char* scriptname, PrecacheEntry *precache, int iarg_02);

void QDECL Scr_PrintScriptRuntimeWarning(const char *fmt, ...);

qboolean Scr_ExecuteMasterResponse(char* s);

void Scr_AddStockFunctions(void);
void Scr_AddStockMethods(void);
