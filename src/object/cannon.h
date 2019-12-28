#ifndef __CANNON_H__
#define __CANNON_H__

#include "../gba/gba.h"
#include "../gba/sprite.h"

#define MAX_BULLETS_PER_CANNON		2
#define NUM_CANNON_BULLET_TICKS		40
extern const FIXED FIXED_BULLET_YVEL;

// externals defined in main.cpp
extern u16 g_nGameplayCharNums[13];
extern const FIXED FIXED_SCREEN_WIDTH;
extern const u8 INITIAL_PADDLE_Y_POS;
extern const u8 BULLET;
extern const u8 CANNON_LEFT;
extern const u8 CANNON_RIGHT;

#define ACTUAL_BULLET_SPRITE_WIDTH			3
#define ACTUAL_BULLET_SPRITE_HEIGHT			3

#define ACTUAL_LEFT_CANNON_SPRITE_WIDTH		5
#define ACTUAL_LEFT_CANNON_SPRITE_HEIGHT	10

#define BULLET_STATE_NULL	0
#define BULLET_STATE_ALIVE	1

typedef struct BULLET_SPRITE
{
	FIXED	ffixPosX;		// x pos of the bullet
	FIXED	ffixPosY;		// y pos of the bullet
	u8		nState;			// state of the bullet (uses BULLET_STATE_ defines above)
	s16		nSpriteIndex;	// sprite index of the bullet
};

typedef struct CANNON_SPRITE
{
	FIXED	ffixPosX;		// x pos of the cannon
	FIXED	ffixPosY;		// y pos of the cannon
	s16		nSpriteIndex;	// sprite index of the cannon
	BULLET_SPRITE	bullets[MAX_BULLETS_PER_CANNON];	// max bullets the cannon can shoot at once
	u16		nCannonTicks;	// counter used to tell when next bullet can be fired
	u8		bCanFire;		// can bullet be fired?
};

// externals defined in cannon.cpp

extern u8 g_bHasCannon;	// does player have the cannon powerup?

// two cannons for all paddle sizes (not for each)
extern CANNON_SPRITE	g_leftCannon;
extern CANNON_SPRITE	g_rightCannon;

// functions

void InitCannons();
//void DeleteCannons();

#endif
