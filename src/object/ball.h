#ifndef __BALL_H__
#define __BALL_H__

#include "../gba/gba.h"
#include "../gba/sprite.h"

extern u16 g_nGameplayCharNums[13];

extern const FIXED INITIAL_BALL_YVEL_MAX;
extern const FIXED INITIAL_BALL_Y_VEL;
extern FIXED INITIAL_BALL_X_VEL;
extern const u8 BALL_INDEX;

#define MAX_ONSCREEN_BALL_SPRITES	8

#define BALL_STATE_NULL		0
#define BALL_STATE_CREATING	1
#define BALL_STATE_ALIVE	2

/*
Angles will be used along with some basic trig for slow and fast ball powerups.
Ok, so i have a right triangle.
The 0 is angle Theta (and also where the ball is before moving each frame.)
The ball will advance a distance of C each frame.
X and Theta are knowns. Use Trig identities to solve for Y, so when player gets
a slow or fast ball powerup, the ball will stay at the same angle (theta) while
slowing down or speeding up.

                /|
               / |
            C /  | Y
             /  _|
            0__|_|
              X

Trig Identities:
  cos(0) = X/C
  sin(0) = Y/C

Solving for C in both identities and substituting gives:
  X/cos(0) = Y/sin(0)

solving for Y gives:
  Y = X sin(0) / cos(0)
*/

typedef struct BALL_SPRITE
{
	FIXED		ffixVelX;		// X velocity
	FIXED		ffixVelY;		// Y velocity
	FIXED		ffixVelC;		// the actual velocity of both x and y combined vectors, where X^2 + Y^2 = C^2
	FIXED		ffixTheta;		// angle Theta (in degrees), the angle from X axis up to C. (will never be >= 90 degrees, because of the trig used above)
	FIXED		ffixPosX;		// X position
	FIXED		ffixPosY;		// Y position
	u8			nState;			// state of the ball: use BALL_STATE_ defines
	s16			nSpriteIndex;	// index into sprite memory
};

extern BALL_SPRITE	g_BallSprites[MAX_ONSCREEN_BALL_SPRITES];
extern u8			g_nBallCount;

void CreateBallSprites();
void InitBallSprites();
void AddBallSprite(FIXED ffixPosX, FIXED ffixPosY, FIXED ffixVelX, FIXED ffixVelY, FIXED ffixVelC, FIXED ffixTheta);
void AddCreatedBallSprites();
void HideBallSprite(u8 nIndex);
void HideAllBallSprites();
void DeleteAllBallSprites();

#endif
