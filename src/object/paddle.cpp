#include "paddle.h"

s8 g_nCurrentPaddleIndex;
u8 g_nLastPaddleIndex;
const FIXED g_nfixPaddleHeight = INT_TO_FIXED(6);

s16		g_nSpriteIndexEndSection[2];

PADDLE	g_Paddles[3];

// one-time initialization of different paddle sizes
void InitPaddles()
{
	g_Paddles[0].nPaddleSection = PADDLE_SECTION_MIN;
	g_Paddles[1].nPaddleSection = PADDLE_SECTION_MID;
	g_Paddles[2].nPaddleSection = PADDLE_SECTION_MAX;

	s16 nScreenWidth = FIXED_INT_PART(FIXED_SCREEN_WIDTH);

	// create one sprite for each the left and right parts of the paddles and set them offscreen
	g_nSpriteIndexEndSection[0] = SpriteCreate(g_nGameplayCharNums[PADDLE_LEFT], nScreenWidth,
										INITIAL_PADDLE_Y_POS, 0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nSpriteIndexEndSection[1] = SpriteCreate(g_nGameplayCharNums[PADDLE_RIGHT], nScreenWidth,
										INITIAL_PADDLE_Y_POS, 0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

	// params that are common amoung all three paddles
	s16 i, j;
	for(i = 0; i < 3; i++)
	{
		g_Paddles[i].nState = PADDLE_STATE_NULL;
		
		// set offscreen
		g_Paddles[i].ffixPosX = FIXED_SCREEN_WIDTH;
		g_Paddles[i].ffixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);

		g_Paddles[i].nfixPaddleWidth = INT_TO_FIXED(g_Paddles[i].nPaddleSection * PADDLE_SECTION_WIDTH) + FIXED_8;

		GeneratePaddleMap(i);

		// create mid section sprites based on size of paddle
		for(j = 0; j < g_Paddles[i].nPaddleSection; j++)
		{
			g_Paddles[i].nSpriteIndexMidSection[j] = SpriteCreate(g_nGameplayCharNums[PADDLE_MID], nScreenWidth,
														INITIAL_PADDLE_Y_POS, 0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_WIDE, SPRITE_COLOR_256);
		}
	}
}

// this is a mapping used for when the ball bounces off the paddle.
// when it bounces off the paddle, it changes the ball's x velocity,
// based on where it hits the paddle.
void GeneratePaddleMap(s8 nPaddleIndex)
{
	// Each paddle_map value represents a theta (angle in degrees) to which the ball will go.
	// Index 17 is halfway, so here's how this will work:
	// At index 17 and up, the ball will bounce to the right
	// At indexes < 17, the ball will bounce to the left.
	// This means that even though there are positive theta values at indexes under 17,
	// it means those theta values are referring to the triangle flipped on the Y axis.

	FIXED nfixAngleMax = BALL_THETA_MAX;
	FIXED nfixAngleMin = BALL_THETA_MIN;

	u8 nWidth = g_Paddles[nPaddleIndex].nPaddleSection * PADDLE_SECTION_WIDTH + 8;
	u8 nWidth2 = nWidth - 1;
	u8 nHalfWidth = nWidth >> 1;
	u8 nLeftHalfWidth = nHalfWidth - 1;
	u8 nRightHalfWidth = nHalfWidth + 1;

	// set halfway point = closest to, but not equal to 90 degrees
	g_Paddles[nPaddleIndex].paddle_map[nHalfWidth] = nfixAngleMax;

	// set end-points = smallest possible theta angle
	g_Paddles[nPaddleIndex].paddle_map[0]  = nfixAngleMin;
	g_Paddles[nPaddleIndex].paddle_map[nWidth2] = nfixAngleMin;

	FIXED ffixAngleOffset = FIXED_DIVIDE(nfixAngleMax - nfixAngleMin, INT_TO_FIXED(nLeftHalfWidth));

	// set the remaining left side paddle_map values
	FIXED ffixNextAngleOffset = nfixAngleMax - ffixAngleOffset;
	s16 i;
	for(i = nLeftHalfWidth; i > 0; i--)
	{
		g_Paddles[nPaddleIndex].paddle_map[i] = ffixNextAngleOffset;
		ffixNextAngleOffset -= ffixAngleOffset;
	}

	// set the remaining right side paddle_map values
	ffixNextAngleOffset = nfixAngleMax - ffixAngleOffset;
	for(i = nRightHalfWidth; i < nWidth2; i++)
	{
		g_Paddles[nPaddleIndex].paddle_map[i] = ffixNextAngleOffset;
		ffixNextAngleOffset -= ffixAngleOffset;
	}
}

void MovePaddle(s8 nPaddleIndex, FIXED ffixPosX, FIXED ffixPosY)
{
	s16 nPosX = FIXED_INT_PART(ffixPosX);
	s16 nPosY = FIXED_INT_PART(ffixPosY);
	s16 nLeftCannonPosX = nPosX;
	s16 nCannonPosY;

	// set pos of leftmost sprite
	SpriteSetPos(g_nSpriteIndexEndSection[0], nPosX, nPosY);
	nPosX += PADDLE_SECTION_END_WIDTH;

	// set pos of middle section sprites
	s8 j;
	for(j = 0; j < g_Paddles[nPaddleIndex].nPaddleSection; j++)
	{
		SpriteSetPos(g_Paddles[nPaddleIndex].nSpriteIndexMidSection[j], nPosX, nPosY);
		nPosX += PADDLE_SECTION_WIDTH;
	}

	// set pos of rightmost sprite
	SpriteSetPos(g_nSpriteIndexEndSection[1], nPosX, nPosY);

	// set pos of cannon sprites
	if(g_bHasCannon == 1)
	{
		g_leftCannon.ffixPosX = ffixPosX;
		g_leftCannon.ffixPosY = ffixPosY + g_nfixPaddleHeight - INT_TO_FIXED(ACTUAL_LEFT_CANNON_SPRITE_HEIGHT);

		g_rightCannon.ffixPosX = ffixPosX + g_Paddles[nPaddleIndex].nfixPaddleWidth - INT_TO_FIXED(ACTUAL_LEFT_CANNON_SPRITE_WIDTH);
		g_rightCannon.ffixPosY = g_leftCannon.ffixPosY;

		nCannonPosY = FIXED_INT_PART(g_leftCannon.ffixPosY);

		SpriteSetPos(g_leftCannon.nSpriteIndex, nLeftCannonPosX, nCannonPosY);
		SpriteSetPos(g_rightCannon.nSpriteIndex, (s16)FIXED_INT_PART(g_rightCannon.ffixPosX), nCannonPosY);
	}
}

void AttachCannons()
{
	g_leftCannon.ffixPosX = g_Paddles[g_nCurrentPaddleIndex].ffixPosX;
	g_leftCannon.ffixPosY = g_Paddles[g_nCurrentPaddleIndex].ffixPosY + g_nfixPaddleHeight - INT_TO_FIXED(ACTUAL_LEFT_CANNON_SPRITE_HEIGHT);

	g_rightCannon.ffixPosX = g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth - INT_TO_FIXED(ACTUAL_LEFT_CANNON_SPRITE_WIDTH);
	g_rightCannon.ffixPosY = g_leftCannon.ffixPosY;

	SpriteSetPos(g_leftCannon.nSpriteIndex, (s16)FIXED_INT_PART(g_leftCannon.ffixPosX), (s16)FIXED_INT_PART(g_leftCannon.ffixPosY));
	SpriteSetPos(g_rightCannon.nSpriteIndex, (s16)FIXED_INT_PART(g_rightCannon.ffixPosX), (s16)FIXED_INT_PART(g_rightCannon.ffixPosY));
}
