#include "ball.h"

BALL_SPRITE	g_BallSprites[MAX_ONSCREEN_BALL_SPRITES];
u8			g_nBallCount = 0;

// Creates all possible ball sprites, and sets them offscreen
void CreateBallSprites()
{
	s16 nIndex;

	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		nIndex = SpriteCreate(g_nGameplayCharNums[BALL_INDEX], (s16)SCREEN_WIDTH, (s16)SCREEN_HEIGHT,
							  0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

		g_BallSprites[i].nSpriteIndex = nIndex;
	}

	InitBallSprites();
}

// sets all balls to null state and offscreen
void InitBallSprites()
{
	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		g_BallSprites[i].ffixVelX = 0;
		g_BallSprites[i].ffixVelY = 0;
		g_BallSprites[i].ffixVelC = 0;
		g_BallSprites[i].ffixTheta = 0;
		g_BallSprites[i].ffixPosX = INT_TO_FIXED(SCREEN_WIDTH);
		g_BallSprites[i].ffixPosY = INT_TO_FIXED(SCREEN_HEIGHT);
		g_BallSprites[i].nState = BALL_STATE_NULL;

		if(g_BallSprites[i].nSpriteIndex != -1)
			SpriteSetPos(g_BallSprites[i].nSpriteIndex, (s16)SCREEN_WIDTH, (s16)SCREEN_HEIGHT);
	}

	g_nBallCount = 0;
}

// if available, creates a ball sprite. (sets "creating" state and doesn't actually add the ball to the game yet)
void AddBallSprite(FIXED ffixPosX, FIXED ffixPosY, FIXED ffixVelX, FIXED ffixVelY, FIXED ffixVelC, FIXED ffixTheta)
{
	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		if(g_BallSprites[i].nState == BALL_STATE_NULL && g_BallSprites[i].nSpriteIndex != -1)
		{
			// add ball sprite to game
			g_BallSprites[i].nState = BALL_STATE_CREATING;
			g_BallSprites[i].ffixVelX = ffixVelX;
			g_BallSprites[i].ffixVelY = ffixVelY;
			g_BallSprites[i].ffixVelC = ffixVelC;
			g_BallSprites[i].ffixTheta = ffixTheta;
			g_BallSprites[i].ffixPosX = ffixPosX;
			g_BallSprites[i].ffixPosY = ffixPosY;

			SpriteSetPos(g_BallSprites[i].nSpriteIndex, FIXED_INT_PART(ffixPosX), FIXED_INT_PART(ffixPosY));

			g_nBallCount++;
			return;
		}
	}
}

// Sets any balls in the creating state, to the alive state
// This is neccessary so the multiball powerup works correctly.
// Calling AddBallSprite won't actually add the ball to the game.
// You must call this function after your call(s) to AddBallSprite().
void AddCreatedBallSprites()
{
	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		if(g_BallSprites[i].nState == BALL_STATE_CREATING)
		{
			g_BallSprites[i].nState = BALL_STATE_ALIVE;
		}
	}
}

// moves one ball sprite object offscreen (does not delete the sprite memory)
void HideBallSprite(u8 nIndex)
{
	if(g_BallSprites[nIndex].nSpriteIndex != -1)
	{
		SpriteSetPos(g_BallSprites[nIndex].nSpriteIndex, (s16)SCREEN_WIDTH, (s16)SCREEN_HEIGHT);
		g_BallSprites[nIndex].nState = BALL_STATE_NULL;

		g_nBallCount--;
	}
}

// hides all ball sprites objects offscreen (doesn't delete the sprite memory)
void HideAllBallSprites()
{
	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		HideBallSprite(i);
	}
}

// deletes the ball sprite objects (as well as their sprite memory)
void DeleteAllBallSprites()
{
	for(u8 i = 0; i < MAX_ONSCREEN_BALL_SPRITES; i++)
	{
		if(g_BallSprites[i].nSpriteIndex != -1)
		{
			DeleteSprite(g_BallSprites[i].nSpriteIndex);
			g_BallSprites[i].nState = BALL_STATE_NULL;
			g_BallSprites[i].nSpriteIndex = -1;
		}
	}

	g_nBallCount = 0;
}
