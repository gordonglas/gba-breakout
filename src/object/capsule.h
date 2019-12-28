#ifndef __CAPSULE_TABLE_H__
#define __CAPSULE_TABLE_H__

#include "../gba/gba.h"
#include "../gba/sprite.h"

#define MAX_CAPSULE_SPRITES_ON_SCREEN	2

// percentages of type of capsules dropped (not the percentage a capsule is actually dropped or not)
#define PERCENT_CAPSULE_CANNON		10
#define PERCENT_CAPSULE_GROW		15
#define PERCENT_CAPSULE_MULTIBALL	10
#define PERCENT_CAPSULE_SHRINK		25
#define PERCENT_CAPSULE_SLOWBALL	15
#define PERCENT_CAPSULE_FASTBALL	25

// values of the look-up table (these also correspond to the index into the char num array in main.cpp)
#define CAPSULE_NULL		0
#define CAPSULE_CANNON		7
#define CAPSULE_GROW		8
#define CAPSULE_MULTIBALL	9
#define CAPSULE_SHRINK		10
#define CAPSULE_SLOWBALL	11
#define CAPSULE_FASTBALL	12

// capsule functions - are called when a player gets a capsule powerup
// nVal is the sprite array index
extern void OnPowerupCapsuleCannon();
extern void OnPowerupCapsuleGrow();
extern void OnPowerupCapsuleMultiball();
extern void OnPowerupCapsuleShrink();
extern void OnPowerupCapsuleSlowball();
extern void OnPowerupCapsuleFastball();

// array of function pointers... a function for each capsule.
// the functions are located in main.cpp
// indexes correspond to CAPSULE_ defines above.
extern fp fpCapsuleTable[13];

typedef struct CAPSULE_SPRITE
{
	u8 nSpriteState;	// sprite state (uses SPRITE_STATE_... macros in sprite.h)
	s16 nSpriteIndex;	// index into gba's sprite array
	u8 nCapsuleType;	// type of capsule this is (uses CAPSULE_... macros)
	FIXED ffixPosX;		// x position
	FIXED ffixPosY;		// y position
};

extern u8 g_nCapsuleTable[255];

extern CAPSULE_SPRITE g_CapsuleSprites[MAX_CAPSULE_SPRITES_ON_SCREEN];

// uses percentages from defines to create the look-up table
void InitCapsuleTable();
void InitCapsuleSprites();
s16 CheckForAvailableCapsuleIndex();
void CreateCapsuleSprite(u8 nCapsuleType, s16 nSpriteIndex, s16 nCapsuleIndex, s16 nPosX, s16 nPosY);
void DeleteCapsuleSprite(s16 nCapsuleIndex);
void DeleteAllCapsuleSprites();

#endif
