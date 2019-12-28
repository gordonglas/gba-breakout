#include "capsule.h"

fp fpCapsuleTable[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	OnPowerupCapsuleCannon,
	OnPowerupCapsuleGrow,
	OnPowerupCapsuleMultiball,
	OnPowerupCapsuleShrink,
	OnPowerupCapsuleSlowball,
	OnPowerupCapsuleFastball
};

// A look-up table that contains the values that represent the capules in quantities that represent the percentages above.
// The first indexes represent the percentages of which capsules are chosen, which are defined above.
// The remaining indexes represent no capsule dropped, so changing the defines above will change the percentages dropped vs not dropped.

u8 g_nCapsuleTable[255];

// A table containing "copies" of valid capsule sprite objects
CAPSULE_SPRITE g_CapsuleSprites[MAX_CAPSULE_SPRITES_ON_SCREEN];

// uses percentages from defines to create the look-up table
void InitCapsuleTable()
{
	u8 nLoop;
	u16 n = 0;

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_CANNON; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_CANNON;
		n++;
	}

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_GROW; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_GROW;
		n++;
	}

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_MULTIBALL; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_MULTIBALL;
		n++;
	}

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_SHRINK; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_SHRINK;
		n++;
	}

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_SLOWBALL; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_SLOWBALL;
		n++;
	}

	for(nLoop = 0; nLoop < PERCENT_CAPSULE_FASTBALL; nLoop++)
	{
		g_nCapsuleTable[n] = CAPSULE_FASTBALL;
		n++;
	}

	while(n < 256)
	{
		g_nCapsuleTable[n] = CAPSULE_NULL;
		n++;
	}
}

void InitCapsuleSprites()
{
	for(u8 i = 0; i < MAX_CAPSULE_SPRITES_ON_SCREEN; i++)
	{
		g_CapsuleSprites[i].ffixPosX = INT_TO_FIXED(SCREEN_WIDTH);
		g_CapsuleSprites[i].ffixPosY = INT_TO_FIXED(SCREEN_HEIGHT);
		g_CapsuleSprites[i].nSpriteIndex = -1;
		g_CapsuleSprites[i].nSpriteState = SPRITE_STATE_NULL;
	}
}

s16 CheckForAvailableCapsuleIndex()
{
	for(s16 i = 0; i < MAX_CAPSULE_SPRITES_ON_SCREEN; i++)
	{
		if(g_CapsuleSprites[i].nSpriteState == SPRITE_STATE_NULL)
			return i;
	}

	return -1;
}

void CreateCapsuleSprite(u8 nCapsuleType, s16 nSpriteIndex, s16 nCapsuleIndex, s16 nPosX, s16 nPosY)
{
	g_CapsuleSprites[nCapsuleIndex].nSpriteState = SPRITE_STATE_ALIVE;
	g_CapsuleSprites[nCapsuleIndex].nCapsuleType = nCapsuleType;
	g_CapsuleSprites[nCapsuleIndex].nSpriteIndex = nSpriteIndex;
	g_CapsuleSprites[nCapsuleIndex].ffixPosX = INT_TO_FIXED(nPosX);
	g_CapsuleSprites[nCapsuleIndex].ffixPosY = INT_TO_FIXED(nPosY);
}

void DeleteCapsuleSprite(s16 nCapsuleIndex)
{
	if(g_CapsuleSprites[nCapsuleIndex].nSpriteIndex != -1)
	{
		DeleteSprite(g_CapsuleSprites[nCapsuleIndex].nSpriteIndex);
		g_CapsuleSprites[nCapsuleIndex].nSpriteState = SPRITE_STATE_NULL;
	}
}

void DeleteAllCapsuleSprites()
{
	for(u8 i = 0; i < MAX_CAPSULE_SPRITES_ON_SCREEN; i++)
	{
		DeleteCapsuleSprite(i);
	}

	InitCapsuleSprites();
}
