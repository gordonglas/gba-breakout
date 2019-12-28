#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "../gba/gba.h"
#include "../gba/sprite.h"
#include "cannon.h"

// externals defined in main.cpp
extern const FIXED BALL_THETA_MAX;
extern const FIXED BALL_THETA_MIN;
extern const FIXED FIXED_8;
extern const FIXED FIXED_SCREEN_WIDTH;
extern u16 g_nGameplayCharNums[13];
extern const u8 PADDLE_MID;
extern const u8 PADDLE_LEFT;
extern const u8 PADDLE_RIGHT;
extern const u8 INITIAL_PADDLE_Y_POS;
extern const FIXED INITIAL_PADDLE_HVEL_FACTOR;

// externals defined in paddle.cpp
extern s8 g_nCurrentPaddleIndex;		// index int g_Paddles array of current paddle
extern u8 g_nLastPaddleIndex;			// used to tell what paddle size we are fading from
extern const FIXED g_nfixPaddleHeight;	// y height of paddles
extern FIXED g_ffixPaddleHVelFactor;	// the current horizontal velocity of the paddle

extern s16	 g_nSpriteIndexEndSection[2];	// index into sprite memory for end parts of the paddles

// paddle states
#define PADDLE_STATE_NULL		0
#define PADDLE_STATE_ALIVE		1

// width of middle sections and end sections of the paddle
#define PADDLE_SECTION_WIDTH		10
#define PADDLE_SECTION_END_WIDTH	4

// number of middle sections the paddle can have at the three stages
#define PADDLE_SECTION_MIN		1
#define PADDLE_SECTION_MID		3
#define PADDLE_SECTION_MAX		5

typedef struct PADDLE
{
	FIXED		paddle_map[58];		// paddle map used for collision with ball
	FIXED		nfixPaddleWidth;	// width of the paddle
	FIXED		ffixPosX;			// x position of paddle
	FIXED		ffixPosY;			// y position of paddle
	u8			nState;				// state of the paddle: use PADDLE_STATE_ defines
	u8			nPaddleSection;		// tells us how many mid sections this paddle has (use PADDLE_SECTION_ defines)
	s16			nSpriteIndexMidSection[PADDLE_SECTION_MAX];	// index into sprite memory for middle parts of the paddle
};

extern PADDLE	g_Paddles[3];

// setup functions

// initialization of different paddle sizes
void InitPaddles();
// new method for calculating the paddle mapping - using trig
void GeneratePaddleMap(s8 nPaddleIndex);

// runtime functions

void MovePaddle(s8 nPaddleIndex, FIXED ffixPosX, FIXED ffixPosY);
void AttachCannons();

#endif
