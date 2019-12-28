#include "cannon.h"

u8 g_bHasCannon = 0;

// number of frames to wait until next bullet can be fired
const FIXED FIXED_BULLET_YVEL = FLOAT_TO_FIXED(1.0f);

CANNON_SPRITE	g_leftCannon;
CANNON_SPRITE	g_rightCannon;

void InitCannons()
{
	FIXED nfixInitialPaddleYPos = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);

	// cannon sprites are assigned priority of zero to be drawn on top of the paddles, which are priority one

	// create left cannon sprite
	g_leftCannon.nSpriteIndex = SpriteCreate(g_nGameplayCharNums[CANNON_LEFT],
												(s16)SCREEN_WIDTH, (s16)INITIAL_PADDLE_Y_POS,
												0, 0, 1, SPRITE_SIZE_16, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	// create right cannon sprite
	g_rightCannon.nSpriteIndex = SpriteCreate(g_nGameplayCharNums[CANNON_RIGHT],
												(s16)SCREEN_WIDTH, (s16)INITIAL_PADDLE_Y_POS,
												0, 0, 1, SPRITE_SIZE_16, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

	// set pos of cannons
	g_leftCannon.ffixPosX	= FIXED_SCREEN_WIDTH;
	g_rightCannon.ffixPosX	= FIXED_SCREEN_WIDTH;
	g_leftCannon.ffixPosY	= nfixInitialPaddleYPos;
	g_rightCannon.ffixPosY	= nfixInitialPaddleYPos;

	// initialize counters
	g_leftCannon.nCannonTicks = 0;
	g_rightCannon.nCannonTicks = 0;
	
	// ready the cannons to fire for when the player gets the cannon powerup
	g_leftCannon.bCanFire = 1;
	g_rightCannon.bCanFire = 1;

	// initialize bullet sprites for both left and right cannons
	u8 i;
	for(i = 0; i < MAX_BULLETS_PER_CANNON; i++)
	{
		g_leftCannon.bullets[i].nSpriteIndex = SpriteCreate(g_nGameplayCharNums[BULLET], (s16)SCREEN_WIDTH, (s16)INITIAL_PADDLE_Y_POS,
													0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
		g_rightCannon.bullets[i].nSpriteIndex = SpriteCreate(g_nGameplayCharNums[BULLET], (s16)SCREEN_WIDTH, (s16)INITIAL_PADDLE_Y_POS,
													0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

		g_leftCannon.bullets[i].nState		= BULLET_STATE_NULL;
		g_rightCannon.bullets[i].nState		= BULLET_STATE_NULL;

		g_leftCannon.bullets[i].ffixPosX	= FIXED_SCREEN_WIDTH;
		g_rightCannon.bullets[i].ffixPosX	= FIXED_SCREEN_WIDTH;
		g_leftCannon.bullets[i].ffixPosY	= nfixInitialPaddleYPos;
		g_rightCannon.bullets[i].ffixPosY	= nfixInitialPaddleYPos;
	}
}

/*
void DeleteCannons()
{
	s16 nIndex;

	// delete all bullet sprites
	for(nIndex = MAX_BULLETS_PER_CANNON - 1; nIndex > -1; nIndex--)
	{
		DeleteSprite(g_rightCannon.bullets[nIndex].nSpriteIndex);
		DeleteSprite(g_leftCannon.bullets[nIndex].nSpriteIndex);
	}

	// delete left and right cannon sprites
	DeleteSprite(g_rightCannon.nSpriteIndex);
	DeleteSprite(g_leftCannon.nSpriteIndex);
}
*/
