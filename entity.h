/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).  

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "q_math.h"
#include "q_shared.h"

#define g_entities ((gentity_t*)(0x841ffe0))

typedef int clipHandle_t;

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t		trBase;
	vec3_t		trDelta;			// velocity, etc
} trajectory_t;


typedef struct{
	int	cullDist;
	int	period;
}lerp_loopFx_t;


typedef struct{
	int	bodyPitch;
	int	bodyRoll;
	int	steerYaw;
	int	materialTime;
	int	gunPitch;
	int	gunYaw;
	int	teamAndOwnerIndex;
}lerp_vehicle_t;


typedef struct{
	int	lerp;
}lerp_soundBlend_t;


typedef struct{
	int	launchTime;
}lerp_missile_t;


typedef struct{
	int	leanf;
	int	movementDir;
}lerp_player_t;


typedef struct{
	int	data[6];
}lerp_anonymous_t;

typedef struct {
	int		eFlags;
	trajectory_t	pos;	// for calculating position	0x0c
	trajectory_t	apos;	// for calculating angles	0x30
	union{
		lerp_anonymous_t	anonymous;
		lerp_player_t		player;
		lerp_missile_t		missile;
		lerp_soundBlend_t	soundBlend;
		lerp_loopFx_t		loopFx;
		lerp_vehicle_t		vehicle;
	}u;
}lerp_t;

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s {//Confirmed names and offsets but not types

	int		number;			// entity index	//0x00
	int		eType;			// entityType_t	//0x04

	lerp_t		lerp;

	int		time2;			//0x70


	int		otherEntityNum;		//0x74 shotgun sources, etc
	int		attackerEntityNum;	//0x78

	int		groundEntityNum;	//0x7c -1 = in air

	int		loopSound;		//0x80 constantly loop this sound
	int		surfType;		//0x84


	clipHandle_t	index;			//0x88
	int		clientNum;		//0x8c 0 to (MAX_CLIENTS - 1), for players and corpses
	int		iHeadIcon;		//0x90
	int		iHeadIconTeam;		//0x94

	int		solid;			//0x98 for client side prediction, trap_linkentity sets this properly	0x98

	int		eventParm;		//0x9c impulse events -- muzzle flashes, footsteps, etc
	int		eventSequence;		//0xa0

	vec4_t		events;			//0xa4
	vec4_t		eventParms;		//0xb4

	// for players
	int		weapon;			//0xc4 determines weapon and flash model, etc
	int		weaponModel;		//0xc8
	int		legsAnim;		//0xcc mask off ANIM_TOGGLEBIT
	int		torsoAnim;		//0xd0 mask off ANIM_TOGGLEBIT

	union{
		int	helicopterStage;	//0xd4
	}un1;
	int		un2;			//0xd8
	int		fTorsoPitch;		//0xdc
	int		fWaistPitch;		//0xe0
	vec4_t		partBits;		//0xe4
} entityState_t;	//sizeof(entityState_t): 0xf4


typedef struct {
	//entityState_t	s;				//Duplicated struct is removed
	byte		linked;				//0xf4 qfalse if not in any good cluster

	byte		bmodel;				//0xf5 if false, assume an explicit mins / maxs bounding box
							// only set by trap_SetBrushModel
	short		unknown1;
	int		unknown2[4];

	vec3_t		mins, maxs;		//0x108  //0x114  from SharedEntity_t

	int		contents;		// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
						// a non-solid entity should set to 0

	vec3_t		absmin, absmax;		//0x124  //0x130 derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t		currentOrigin;		//0x13c
	vec3_t		currentAngles;		//0x148

	// when a trace call is made and passEntityNum != ENTITYNUM_NONE,
	// an ent will be excluded from testing if:
	// ent->s.number == passEntityNum	(don't interact with self)
	// ent->r.ownerNum == passEntityNum	(don't interact with your own missiles)
	// entity[ent->r.ownerNum].r.ownerNum == passEntityNum	(don't interact with other missiles from owner)
	short			ownerNum;	//0x154
	short			unknown3;
	int			eventTime;
} entityShared_t;


// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
typedef struct {
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game
} sharedEntity_t;

