/*
  by Gordon Glas - Sept 18, 2002
  https://overworldsoft.com
  (Formerly Illusive Studios)
*/

#include <stdio.h>
#include <string.h>

#include "gba/gba.h"
#include "gba/sprite.h"
#include "gba/background.h"
#include "gba/interupts.h"
#include "gba/font.h"
#include "gba/util.h"

#include "../music/GbaPlayer.h"
#include "sound/modfiles.h"

//#include "gba/sfx.h"
//#include "gba/sfx.c"

// game objects & look-up tables
#include "object/cloud.h"
#include "object/cannon.h"
#include "object/paddle.h"
#include "object/level.h"
#include "object/ball.h"
#include "object/capsule.h"

// sprite and palette data
#include "art/IllusiveLogo_0_0.h"
#include "art/IllusiveLogo_1_0.h"
#include "art/IllusiveLogo_2_0.h"
#include "art/IllusiveLogo_0_1.h"
#include "art/IllusiveLogo_1_1.h"
#include "art/IllusiveLogo_2_1.h"
#include "art/blockout_logo1.h"
#include "art/blockout_logo2.h"
#include "art/blockout_logo3.h"
#include "art/blockout_logo4.h"
#include "art/title_bg_tiles.h"
#include "art/game_bg_tiles.h"
#include "art/ball_game_sprite.h"
//#include "art/paddle_game_sprite.h"
#include "art/paddle_game_sprite_left.h"
#include "art/paddle_game_sprite_mid.h"
#include "art/paddle_game_sprite_right.h"
#include "art/bullet_sprite.h"
#include "art/cannon_left_sprite.h"
#include "art/cannon_right_sprite.h"
#include "art/capsule_sprite_cannon.h"
#include "art/capsule_sprite_grow.h"
#include "art/capsule_sprite_multiball.h"
#include "art/capsule_sprite_shrink.h"
#include "art/capsule_sprite_speeddown.h"
#include "art/capsule_sprite_speedup.h"

// map data
#include "map/start_game_map.h"
#include "map/test_sky_map.h"

// sound data
//#include "sound/logo.h" // logo fade sound

// function ptr for main game functions
void (*fpGame)();

// setup functions
void InitBackgrounds();
void EnableBackgrounds();

// logo functions
void LogoCreate();
void LogoFadeIn();
void LogoPause();
void LogoFadeOut();
void LogoDelete();

// menu functions
void MenuCreate();
void MenuScrollTitle();
void MenuLoop();
void MenuDelete();

// gameplay functions
void GameplayCountdown();
void GameplayCreate();
void GameplayLoop();
void GameplayDelete();

// gameplay helper functions
void UpdateCapsules();
void ScrollBackground();
void CheckCapsuleCreate(u16 nBlockTileX, u16 nBlockTileY, u8 nModulus);
void LoadFirstLevel();
void LoadNextLevel();
void LoadLevelData();
void ResetBallAndPaddlePos();
void DrawStats();
void DeathSequence();
void GameOver();
void PauseGame(int nSeconds);

// flag that will tell us when to draw (is set to 1 in vblank interrupt)
// it's your responsibility to set it back to zero when done drawing.
u8 g_bDraw = 0;

// timer vars
FIXED g_ffixTransparencyFactor;
FIXED g_ffixTarget2AlphaFactor;
u32 g_tick;
u32 g_lastTick;
u32 g_waitTicks;
u32 g_timeRet;

// logo sprite indexes
s16 g_nIllusiveLogoSpriteIndexes[6];
s16 g_nMenuLogoSpriteIndexes[4];

// menu logo x position vars (y is hardcoded as 5)
FIXED g_ifixMenuLogo1X = INT_TO_FIXED(240);
FIXED g_ifixMenuLogo2X = INT_TO_FIXED(240);
FIXED g_ifixMenuLogo3X = INT_TO_FIXED(240);
FIXED g_ifixMenuLogo4X = INT_TO_FIXED(240);
const FIXED g_ffixMenuLogoScrollFactor = FLOAT_TO_FIXED(0.14f);

// some clouds for title screen
Cloud g_cloud1;
Cloud g_cloud2;
Cloud g_cloud3;

// initial ball velocities

// max and min y velocity of ball - must be positive
const FIXED INITIAL_BALL_YVEL_MIN	= FLOAT_TO_FIXED(1.5f);
const FIXED INITIAL_BALL_YVEL_MAX	= FLOAT_TO_FIXED(3.0f);
// max and min x velocity of ball (Will be calculated at startup)
FIXED INITIAL_BALL_XVEL_MIN			= 0;
FIXED INITIAL_BALL_XVEL_MAX			= 0;
// initial y velocity of ball
const FIXED INITIAL_BALL_Y_VEL		= FLOAT_TO_FIXED(2.5f);
// initial y velocity of ball (Will be calculated at startup)
FIXED INITIAL_BALL_X_VEL			= 0;
// initial C vector of the ball (combined x and y vector of the ball. Will be calculated at startup)
FIXED FIXED_INITIAL_BALL_C_VEL		= 0;
// max and min angle theta of ball (in degrees)
const FIXED BALL_THETA_MAX			= INT_TO_FIXED(89);
const FIXED BALL_THETA_MIN			= INT_TO_FIXED(35);
// initial angle theta of ball
const FIXED FIXED_INITIAL_BALL_THETA = INT_TO_FIXED(70);
// difference in angle of new balls for multiball powerup
const FIXED FIXED_THETA_MULTIBALL_DELTA = INT_TO_FIXED(7);

// paddle pos
const u8 INITIAL_PADDLE_Y_POS = 130;
FIXED g_ffixDeathPaddleYFactor = FLOAT_TO_FIXED(0.6f);
FIXED g_ffixPaddleAlphaBlendFactor = FLOAT_TO_FIXED(0.4f);
FIXED g_ffixBGAlphaBlendFactor = FLOAT_TO_FIXED(0.4f);

// paddle velocity
// how fast the paddle moves horizontally both left and right - it must always be positive
const FIXED INITIAL_PADDLE_HVEL_FACTOR = FLOAT_TO_FIXED(3.9f);

FIXED ballHitPaddlePos = 0;

// background offsets
FIXED g_ffixBgXOffset = 0;
FIXED g_ffixBgYOffset = 0;
// background x & y velocity
#define INITIAL_BG_VEL				FLOAT_TO_FIXED(0.2f)
FIXED g_fxhvel = INITIAL_BG_VEL;
FIXED g_fxvvel = INITIAL_BG_VEL;

// number of tiles remaining for each level
u16 g_NumBlocksRemaining;

// lives (max of 9)
#define TOTAL_LIVES		3
u8 g_lives = TOTAL_LIVES;

// scoring defines
#define SCORE_PADDLE	3
#define SCORE_BLOCK		10
// pass level number of the level that was beaten
#define SCORE_LEVEL(n)	(n * 100)
u32 g_score = 0;

// not used at the moment
//u8 g_bGamePaused = 0;

// index vars
u16* pMem;
u16 loop;

// random 8 bit number
u8 g_nRand8 = 0;

// indexes into char num array
const u8 PADDLE_MID   = 0;
const u8 PADDLE_LEFT  = 1;
const u8 PADDLE_RIGHT = 2;
const u8 BALL_INDEX   = 3;
const u8 BULLET       = 4;
const u8 CANNON_LEFT  = 5;
const u8 CANNON_RIGHT = 6;

s32 g_nPaddleMapIndex = 0;

// global character numbers used for gameplay sprites
u16 g_nGameplayCharNums[13];

// constant fixed globals (optimizations)
const FIXED FIXED_SCREEN_WIDTH   = INT_TO_FIXED(SCREEN_WIDTH);
const FIXED FIXED_SCREEN_HEIGHT  = INT_TO_FIXED(SCREEN_HEIGHT);
const FIXED FIXED_CAPSULE_WIDTH  = INT_TO_FIXED(16);
const FIXED FIXED_CAPSULE_HEIGHT = INT_TO_FIXED(8);
const FIXED FIXED_BALL_WIDTH     = INT_TO_FIXED(actual_ball_sprite_WIDTH);
const FIXED FIXED_BALL_HEIGHT    = INT_TO_FIXED(actual_ball_sprite_HEIGHT);
const FIXED FIXED_CAPSULE_Y_VEL  = FLOAT_TO_FIXED(1.0f);
const FIXED FIXED_1              = INT_TO_FIXED(1);
const FIXED FIXED_2              = INT_TO_FIXED(2);
const FIXED FIXED_3              = INT_TO_FIXED(3);
const FIXED FIXED_8              = INT_TO_FIXED(8);

// distance to move in y direction when getting slow ball / fast ball powerups (allows for three different speeds)
FIXED POWERUP_BALL_Y_VEL_FACTOR;

s16 nNewCapsuleSpriteIndex = -2;	// DEBUG

// start of execution
int main()
{
	// calculate min x velocity
	// first, set y vel to min, and angle theta to min to get the hypotinuse (C vel) at the smallest velocity possible.
	// then, use this new C vel, along with maximum angle theta to get the min X vel
	g_ffixTheta = BALL_THETA_MIN;
	g_ffixYVel  = INITIAL_BALL_YVEL_MIN;
	CalcTrig_CX();
	g_ffixTheta = BALL_THETA_MAX;
	CalcTrig_XY();
	INITIAL_BALL_XVEL_MIN = g_ffixXVel;

	// calculate max x velocity
	// first, set y vel to max, and angle theta to max to get the hypotinuse (C vel)
	// then, use C vel, along with minimum angle theta to get the max X vel
	g_ffixTheta = BALL_THETA_MAX;
	g_ffixYVel  = INITIAL_BALL_YVEL_MAX;
	CalcTrig_CX();
	g_ffixTheta = BALL_THETA_MIN;
	CalcTrig_XY();
	INITIAL_BALL_XVEL_MAX = g_ffixXVel;

	// calculate initial C velocity and X velocity of the ball
	g_ffixTheta = FIXED_INITIAL_BALL_THETA;
	g_ffixYVel  = INITIAL_BALL_Y_VEL;
	CalcTrig_CX();
	FIXED_INITIAL_BALL_C_VEL = g_ffixCVel;
	INITIAL_BALL_X_VEL		 = g_ffixXVel;

	// allow for 3 different ball speeds...
	POWERUP_BALL_Y_VEL_FACTOR = FIXED_DIVIDE(INITIAL_BALL_YVEL_MAX - INITIAL_BALL_YVEL_MIN, FIXED_2);

	// set videomode 0, enable backgrounds 0, 1, and 2, enable sprites,
	// and enable 1-dimensional sprite memory
	SetMode( MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D );

	//init_sfx_system();

	// init mod player
	MP_InitArm();
	EnableInterupts( INT_DMA1 | INT_VBLANK );
	MP_PlayModuleArm(GetModFile(MOD_BLADERAVER), 1);

	// set all sprites to be offscreen
	DeleteAllSprites();

	InitBackgrounds();
	EnableBackgrounds();

	// set up capsule look-up table
	// (to see if a capsule is dropped after the ball hits a block, and to see which capsule is dropped)
	InitCapsuleTable();
	InitCapsuleSprites();

	SetupLevels();

	fpGame = LogoCreate;

	// enter game loop
	while(1)
		fpGame();

	return 0;
}

void InitBackgrounds()
{
	while(REG_VCOUNT<160);	// wait for vsync

	// set bg1 to black in order to "clear screen" for transparency.
	// without this here, the font will not be transparent.
	BGFillMap(31, 47);	// 47 = black tile			bg2
	BGFillMap(30, 47);	// 47 = black tile			bg1
	BGFillMap(29, 0);	// 0 = transparent tile		bg0
}

void EnableBackgrounds()
{
	// enable backgrounds
	REG_BG0CNT = COLOR_256 | TXTBG_256_256;
	REG_BG1CNT = COLOR_256 | TXTBG_256_256;
	REG_BG2CNT = COLOR_256 | TXTBG_256_256;

	BGSetCharBlock(REG_BG0CNT, 1);
	BGSetCharBlock(REG_BG1CNT, 0);
	BGSetCharBlock(REG_BG2CNT, 0);

	BGSetScreenBlock(REG_BG0CNT, 29);
	BGSetScreenBlock(REG_BG1CNT, 30);
	BGSetScreenBlock(REG_BG2CNT, 31);

	BGSetPriority(REG_BG0CNT, 0);
	BGSetPriority(REG_BG1CNT, 2);
	BGSetPriority(REG_BG2CNT, 3);
}

void LogoCreate()
{
	// setup fade-in for logo sprites
	g_ffixTransparencyFactor = FLOAT_TO_FIXED(16);
	REG_COLEY =	(16<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);	// transparency factor (can be 0-16)
	REG_BLDMOD = TARGET1_OBJPLAIN | TARGET2_BACKPLAIN | MODE_FADEOUT;

	// set character base block 0 to font's bitmap data
	BGLoadImage(0, title_bg_tilesData, title_bg_tiles_WIDTH, title_bg_tiles_HEIGHT, 256);

	// set character base block 2 to font's bitmap data (this will be loaded all the time for gameplay fonts)
	BGLoadImage(1, title_bg_tilesData, title_bg_tiles_WIDTH, title_bg_tiles_HEIGHT, 256);

	// set bg pal to title screen background tiles' pal
	BGSetPalette(game_bg_tilesPalette);

	// copy illusive logo palette into sprite palette mem
	SpriteSetPalette(IllusiveStudios_logo_pal);

	// load illusive logo sprite images
	u16 nCharNums[6];
	nCharNums[0] = SpriteLoadImage(IllusiveLogo_0_0Data, 64, 64, 256);
	nCharNums[1] = SpriteLoadImage(IllusiveLogo_1_0Data, 64, 64, 256);
	nCharNums[2] = SpriteLoadImage(IllusiveLogo_2_0Data, 64, 64, 256);
	nCharNums[3] = SpriteLoadImage(IllusiveLogo_0_1Data, 64, 64, 256);
	nCharNums[4] = SpriteLoadImage(IllusiveLogo_1_1Data, 64, 64, 256);
	nCharNums[5] = SpriteLoadImage(IllusiveLogo_2_1Data, 64, 64, 256);

	// at this point, I've used about 24576 bytes = aproximately 24k out of gba's 32k total.
	// yikes... kinda small, huh? =)
	// although, this is only sprite memory, and I'll be using tiles in background memory, which
	// is separate from sprite memory. Not to mention sprites will be much smaller during actual gameplay.

	// create sprites that reference the images by character number
	g_nIllusiveLogoSpriteIndexes[0] = SpriteCreate(nCharNums[0], 36,  41, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nIllusiveLogoSpriteIndexes[1] = SpriteCreate(nCharNums[1], 100, 41, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nIllusiveLogoSpriteIndexes[2] = SpriteCreate(nCharNums[2], 164, 41, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nIllusiveLogoSpriteIndexes[3] = SpriteCreate(nCharNums[3], 36,  76, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nIllusiveLogoSpriteIndexes[4] = SpriteCreate(nCharNums[4], 100, 76, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nIllusiveLogoSpriteIndexes[5] = SpriteCreate(nCharNums[5], 164, 76, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

	// set up transparency

	g_lastTick = 0;
	g_waitTicks = 255; // how long we should wait while logo is displayed

	//Enable timers
	//REG_TM2D = 41950; // 41950 (65536 - 16386) is every second (using frequency 1024)
	REG_TM2CNT = TIME_FREQUENCY_SYSTEM | TIMER_ENABLED;
	REG_TM3CNT = TIME_OVERFLOW | TIMER_ENABLED;

	//play_sfx(&logo);

	fpGame = LogoFadeIn;
}

void LogoFadeIn()
{
	g_tick = REG_TM3D;

	if(g_tick - g_lastTick > 0)
	{
		// the interval has passed
		if(g_ffixTransparencyFactor >= FLOAT_TO_FIXED(0.06))
			g_ffixTransparencyFactor -= FLOAT_TO_FIXED(0.06);
		else
			fpGame = LogoPause;
	}

	if(g_lastTick != g_tick)
	{
		g_lastTick = g_tick;
		REG_COLEY = (16<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);
	}
}

void LogoPause()
{
	g_tick = REG_TM3D;

	if(g_tick - g_lastTick > 0)
	{
		// the interval has passed
		if(g_waitTicks > 0)
			g_waitTicks--;
		else
			fpGame = LogoFadeOut;
	}

	if(g_lastTick != g_tick)
		g_lastTick = g_tick;
}

void LogoFadeOut()
{
	g_tick = REG_TM3D;

	if(g_tick - g_lastTick > 0)
	{
		// the interval has passed
		if(g_ffixTransparencyFactor <= FLOAT_TO_FIXED(15.94))
			g_ffixTransparencyFactor += FLOAT_TO_FIXED(0.06);
		else
		{
			LogoDelete();
			return;
		}
	}

	if(g_lastTick != g_tick)
	{
		g_lastTick = g_tick;
		REG_COLEY = (16<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);
	}
}

void LogoDelete()
{
	while(REG_VCOUNT<160);	// wait for vsync

	// zero out logo sprites
	for(s16 i = 5; i > -1; i--)
	{
		DeleteSprite(g_nIllusiveLogoSpriteIndexes[i]);
	}

	// disable timers
	REG_TM2CNT = 0;
	REG_TM3CNT = 0;

	MenuCreate();
}

void MenuCreate()
{
	InitSpriteMemory();

	// load blockout logo pal
	SpriteSetPalette(blockout_logoPalette);

	// load blockout title images
	u16 nCharNums[4];
	nCharNums[0] = SpriteLoadImage(blockout_logo1Data, 64, 64, 256);
	nCharNums[1] = SpriteLoadImage(blockout_logo2Data, 64, 64, 256);
	nCharNums[2] = SpriteLoadImage(blockout_logo3Data, 64, 64, 256);
	nCharNums[3] = SpriteLoadImage(blockout_logo4Data, 64, 64, 256);

	// set sprites offscreen for now, since they will be scrolled in from the right.
	g_nMenuLogoSpriteIndexes[0] = SpriteCreate(nCharNums[0], 240, 5, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nMenuLogoSpriteIndexes[1] = SpriteCreate(nCharNums[1], 240, 5, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nMenuLogoSpriteIndexes[2] = SpriteCreate(nCharNums[2], 240, 5, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	g_nMenuLogoSpriteIndexes[3] = SpriteCreate(nCharNums[3], 240, 5, 0, 0, 1, SPRITE_SIZE_64, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

	// reset global logo sprite positions
	g_ifixMenuLogo1X = INT_TO_FIXED(240);
	g_ifixMenuLogo2X = INT_TO_FIXED(240);
	g_ifixMenuLogo3X = INT_TO_FIXED(240);
	g_ifixMenuLogo4X = INT_TO_FIXED(240);

	LoadCloudImages();
	// the cloud sprites - Don't forget that each cloud takes up 3 consecutive sprites
	g_cloud1.Create(0, 3);
	g_cloud1.SetVel(0.01f, 0);
	g_cloud2.Create(130, 10);
	g_cloud2.SetVel(0.009f, 0);
	g_cloud3.Create(60, 40);
	g_cloud3.SetVel(0.005f, 0);

	// start timer for scrolling the title logo
	//REG_TM2CNT = TIME_FREQUENCY_SYSTEM | TIMER_ENABLED;
	//REG_TM3CNT = TIME_OVERFLOW | TIMER_ENABLED;

	//while(REG_VCOUNT<160);	// wait for vsync

	// load sky map
	BGLoadMap(30, test_sky_map);

	fpGame = MenuScrollTitle;
}

void MenuScrollTitle()
{
	if(g_ifixMenuLogo1X >= INT_TO_FIXED(5)+g_ffixMenuLogoScrollFactor)
	{
		g_ifixMenuLogo1X -= g_ffixMenuLogoScrollFactor;
		SpriteSetPos(0, FIXED_INT_PART(g_ifixMenuLogo1X), 5);
		if(g_ifixMenuLogo1X <= INT_TO_FIXED(175))
		{
			g_ifixMenuLogo2X = g_ifixMenuLogo1X + INT_TO_FIXED(64);
			SpriteSetPos(1, FIXED_INT_PART(g_ifixMenuLogo2X), 5);
		}
		if(g_ifixMenuLogo2X <= INT_TO_FIXED(175))
		{
			g_ifixMenuLogo3X = g_ifixMenuLogo2X + INT_TO_FIXED(64);
			SpriteSetPos(2, FIXED_INT_PART(g_ifixMenuLogo3X), 5);
		}
		if(g_ifixMenuLogo3X <= INT_TO_FIXED(175))
		{
			g_ifixMenuLogo4X = g_ifixMenuLogo3X + INT_TO_FIXED(64);
			SpriteSetPos(3, FIXED_INT_PART(g_ifixMenuLogo4X), 5);
		}
	}
	else
	{
		//while(REG_VCOUNT<160);	// wait for vsync

		// load Start Game map
		BGLoadMap(29, start_game_map);

		fpGame = MenuLoop;
	}

	// disable transparency
	REG_BLDMOD = 0;

	g_cloud1.Update();
	g_cloud2.Update();
	g_cloud3.Update();
}

void MenuLoop()
{
	if(KEYDOWN(KEY_START))
	{
		MenuDelete();
		GameplayCreate();
		return;
	}

	g_cloud1.Update();
	g_cloud2.Update();
	g_cloud3.Update();
}

void MenuDelete()
{
	// disable timers
	//REG_TM2CNT = 0;
	//REG_TM3CNT = 0;

	g_cloud3.Delete();
	g_cloud2.Delete();
	g_cloud1.Delete();

	// zero out menu sprites
	DeleteSprite(g_nMenuLogoSpriteIndexes[3]);
	DeleteSprite(g_nMenuLogoSpriteIndexes[2]);
	DeleteSprite(g_nMenuLogoSpriteIndexes[1]);
	DeleteSprite(g_nMenuLogoSpriteIndexes[0]);

	InitBackgrounds();
}

void GameplayCreate()
{
	InitSpriteMemory();

	// set game tile image data
	BGLoadImage(0, game_bg_tilesData, game_bg_tiles_WIDTH, game_bg_tiles_HEIGHT, 256);

	// set bg pal to game screen background tiles' pal
	BGSetPalette(game_bg_tilesPalette);

	g_nGameplayCharNums[PADDLE_MID]			= SpriteLoadImage(paddle_game_sprite_midData, 16, 8, 256);		// load middle section of paddle sprite bitmap data
	g_nGameplayCharNums[PADDLE_LEFT]		= SpriteLoadImage(paddle_game_sprite_leftData, 8, 8, 256);		// load left section of paddle sprite bitmap data
	g_nGameplayCharNums[PADDLE_RIGHT]		= SpriteLoadImage(paddle_game_sprite_rightData, 8, 8, 256);		// load right section of paddle sprite bitmap data
	g_nGameplayCharNums[BALL_INDEX]			= SpriteLoadImage(ball_game_spriteData, 8, 8, 256);				// load ball sprite bitmap data
	g_nGameplayCharNums[BULLET]				= SpriteLoadImage(bullet_spriteData, 8, 8, 256);				// load bullet sprite bitmap data
	g_nGameplayCharNums[CANNON_LEFT]		= SpriteLoadImage(cannon_left_spriteData, 16, 16, 256);			// load left cannon sprite bitmap data
	g_nGameplayCharNums[CANNON_RIGHT]		= SpriteLoadImage(cannon_right_spriteData, 16, 16, 256);		// load right cannon sprite bitmap data
	g_nGameplayCharNums[CAPSULE_CANNON]		= SpriteLoadImage(capsule_sprite_cannonData, 16, 8, 256);		// load cannon capsule sprite bitmap data
	g_nGameplayCharNums[CAPSULE_GROW]		= SpriteLoadImage(capsule_sprite_growData, 16, 8, 256);			// load grow capsule sprite bitmap data
	g_nGameplayCharNums[CAPSULE_MULTIBALL]	= SpriteLoadImage(capsule_sprite_multiballData, 16, 8, 256);	// load multiball capsule sprite bitmap data
	g_nGameplayCharNums[CAPSULE_SHRINK]		= SpriteLoadImage(capsule_sprite_shrinkData, 16, 8, 256);		// load shrink capsule sprite bitmap data
	g_nGameplayCharNums[CAPSULE_SLOWBALL]	= SpriteLoadImage(capsule_sprite_speeddownData, 16, 8, 256);	// load speed-down capsule sprite bitmap data
	g_nGameplayCharNums[CAPSULE_FASTBALL]	= SpriteLoadImage(capsule_sprite_speedupData, 16, 8, 256);		// load speed-up capsule sprite bitmap data

	// set gameplay sprite palette
	SpriteSetPalette(game_sprites_Palette);

	// create cannon and bullet sprites
	InitCannons();

	// create paddle sprites
	InitPaddles();

	// create ball sprites
	CreateBallSprites();

	// load map of first level and do countdown
	LoadFirstLevel();
}

void GameplayCountdown()
{
	g_tick = 3;
	g_timeRet = g_lastTick = REG_TM3D = 1;

	// enable timers to overflow every second (using TIME_FREQUENCY_256)
	REG_TM2CNT = TIME_FREQUENCY_256 | TIMER_ENABLED;
	REG_TM3CNT = TIME_OVERFLOW | TIMER_ENABLED;

	while(g_tick > 0)
	{
		g_timeRet = REG_TM3D;

		// if timer overflowed, a second has passed
		if(g_timeRet - g_lastTick > 0)
		{
			g_tick--;
			g_timeRet = g_lastTick = REG_TM3D = 1;
		}

		// only draw text at vsync
		if(g_bDraw == 1)
		{
			ScrollBackground();

			char szCountdown[2];

			TextClear(29);

			// draw countdown
			sprintf(szCountdown, "%d", g_tick);
			TextToBG(29, 14,  9, szCountdown);

			// draw other player stat info
			DrawStats();

			g_bDraw = 0;
		}

		if(g_lastTick != g_timeRet)
			g_lastTick = g_timeRet;
	}

	REG_TM2CNT = 0;
	REG_TM3CNT = 0;

	// hide the countdown number
	u16* pMapMem = (u16*)ScreenBaseBlock(29);
	pMapMem[302] = 0;	// (9 * 32)+14 = 302

	// time is up, start playing
	fpGame = GameplayLoop;
}

void GameplayLoop()
{
	// "mix up" the values in rand8
	g_nRand8 = rand8();

	if(g_bDraw == 1)
	{
		ScrollBackground();

		TextClear(29);

		DrawStats();

		UpdateCapsules();

		u16 blockTileX;
		u16 blockTileY;
		u16 blockTileIndex;
		pMem = (u16 *)ScreenBaseBlock(30);
		u8 blockHalfTarget;
		u8 nModulus;

		// move paddle based on input
		if(KEYDOWN(KEY_LEFT))
		{
			if(g_Paddles[g_nCurrentPaddleIndex].ffixPosX - INITIAL_PADDLE_HVEL_FACTOR < 0)
				g_Paddles[g_nCurrentPaddleIndex].ffixPosX = 0;
			else
				g_Paddles[g_nCurrentPaddleIndex].ffixPosX -= INITIAL_PADDLE_HVEL_FACTOR;
		}
		else if(KEYDOWN(KEY_RIGHT))
		{
			if(g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth + INITIAL_PADDLE_HVEL_FACTOR < FIXED_SCREEN_WIDTH)
				g_Paddles[g_nCurrentPaddleIndex].ffixPosX += INITIAL_PADDLE_HVEL_FACTOR;
			else
				g_Paddles[g_nCurrentPaddleIndex].ffixPosX = FIXED_SCREEN_WIDTH - FIXED_1 - g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth;
		}

		MovePaddle(g_nCurrentPaddleIndex, g_Paddles[g_nCurrentPaddleIndex].ffixPosX, g_Paddles[g_nCurrentPaddleIndex].ffixPosY);

		// if player has cannon powerup, check if they are firing the cannons
		if(g_bHasCannon == 1)
		{
			if(KEYDOWN(KEY_L))
			{
				if(g_leftCannon.bCanFire == 1)
				{
					// check if a bullet is available
					for(loop = 0; loop < MAX_BULLETS_PER_CANNON; loop++)
					{
						if(g_leftCannon.bullets[loop].nState == BULLET_STATE_NULL)
						{
							// fire cannon
							g_leftCannon.bullets[loop].ffixPosX = g_leftCannon.ffixPosX;
							g_leftCannon.bullets[loop].ffixPosY = g_leftCannon.ffixPosY;
							g_leftCannon.bullets[loop].nState = BULLET_STATE_ALIVE;
							g_leftCannon.nCannonTicks = 0;
							g_leftCannon.bCanFire = 0;
							break;
						}
					}
				}
			}
			
			if(KEYDOWN(KEY_R))
			{
				if(g_rightCannon.bCanFire == 1)
				{
					// check if a bullet is available
					for(loop = 0; loop < MAX_BULLETS_PER_CANNON; loop++)
					{
						if(g_rightCannon.bullets[loop].nState == BULLET_STATE_NULL)
						{
							// fire cannon
							g_rightCannon.bullets[loop].ffixPosX = g_rightCannon.ffixPosX;
							g_rightCannon.bullets[loop].ffixPosY = g_rightCannon.ffixPosY;
							g_rightCannon.bullets[loop].nState = BULLET_STATE_ALIVE;
							g_rightCannon.nCannonTicks = 0;
							g_rightCannon.bCanFire = 0;
							break;
						}
					}
				}
			}

			// increment tick counters
			if(g_leftCannon.bCanFire == 0)
			{
				g_leftCannon.nCannonTicks++;
				// when ticks reach a threashold, the cannon is reloaded
				if(g_leftCannon.nCannonTicks >= NUM_CANNON_BULLET_TICKS)
				{
					g_leftCannon.nCannonTicks = 0;
					g_leftCannon.bCanFire = 1;
				}
			}

			if(g_rightCannon.bCanFire == 0)
			{
				g_rightCannon.nCannonTicks++;
				// when ticks reach a threashold, the cannon is reloaded
				if(g_rightCannon.nCannonTicks >= NUM_CANNON_BULLET_TICKS)
				{
					g_rightCannon.nCannonTicks = 0;
					g_rightCannon.bCanFire = 1;
				}
			}
		}

		// update bullet positions and check collisions of bullets against blocks and top of screen
		for(loop = 0; loop < MAX_BULLETS_PER_CANNON; loop++)
		{
			if(g_leftCannon.bullets[loop].nState == BULLET_STATE_ALIVE)
			{
				// if bullet is offscreen, destroy it
				if(FIXED_INT_PART(g_leftCannon.bullets[loop].ffixPosY + INT_TO_FIXED(ACTUAL_BULLET_SPRITE_HEIGHT)) < 0)
				{
					g_leftCannon.bullets[loop].nState = BULLET_STATE_NULL;
					g_leftCannon.bullets[loop].ffixPosX = FIXED_SCREEN_WIDTH;
					g_leftCannon.bullets[loop].ffixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);
					g_leftCannon.bCanFire = 1;
					SpriteSetPos(g_leftCannon.bullets[loop].nSpriteIndex, FIXED_INT_PART(g_leftCannon.bullets[loop].ffixPosX), FIXED_INT_PART(g_leftCannon.bullets[loop].ffixPosY));
				}
				else	// else, do collision detection
				{
					// use center of top of bullet for collision test
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( (g_leftCannon.bullets[loop].ffixPosX + FIXED_1), FIXED_8 ) );
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( g_leftCannon.bullets[loop].ffixPosY, FIXED_8 ) );

					blockTileIndex = (blockTileY << 5) + blockTileX;

					// find out which 8x8 pixel tile this actually is on the map
					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00)	// if true, we've hit a block!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						// destroy bullet
						g_leftCannon.bullets[loop].nState = BULLET_STATE_NULL;
						g_leftCannon.bullets[loop].ffixPosX = FIXED_SCREEN_WIDTH;
						g_leftCannon.bullets[loop].ffixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);
						g_leftCannon.bCanFire = 1;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
					else
					{
						g_leftCannon.bullets[loop].ffixPosY -= FIXED_BULLET_YVEL;
						if(g_leftCannon.bullets[loop].ffixPosY >= 0)
							SpriteSetPos(g_leftCannon.bullets[loop].nSpriteIndex, FIXED_INT_PART(g_leftCannon.bullets[loop].ffixPosX), FIXED_INT_PART(g_leftCannon.bullets[loop].ffixPosY));
					}
				}
			}

			if(g_rightCannon.bullets[loop].nState == BULLET_STATE_ALIVE)
			{
				// if bullet is offscreen, destroy it
				if(FIXED_INT_PART(g_rightCannon.bullets[loop].ffixPosY + INT_TO_FIXED(ACTUAL_BULLET_SPRITE_HEIGHT)) < 0)
				{
					g_rightCannon.bullets[loop].nState = BULLET_STATE_NULL;
					g_rightCannon.bullets[loop].ffixPosX = FIXED_SCREEN_WIDTH;
					g_rightCannon.bullets[loop].ffixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);
					g_rightCannon.bCanFire = 1;
					SpriteSetPos(g_rightCannon.bullets[loop].nSpriteIndex, FIXED_INT_PART(g_rightCannon.bullets[loop].ffixPosX), FIXED_INT_PART(g_rightCannon.bullets[loop].ffixPosY));
				}
				else	// else, do collision detection
				{
					// use center of top of bullet for collision test
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( (g_rightCannon.bullets[loop].ffixPosX + FIXED_1), FIXED_8 ) );
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( g_rightCannon.bullets[loop].ffixPosY, FIXED_8 ) );

					blockTileIndex = (blockTileY << 5) + blockTileX;

					// find out which 8x8 pixel tile this actually is on the map
					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00)	// if true, we've hit a block!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						// destroy bullet
						g_rightCannon.bullets[loop].nState = BULLET_STATE_NULL;
						g_rightCannon.bullets[loop].ffixPosX = FIXED_SCREEN_WIDTH;
						g_rightCannon.bullets[loop].ffixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);
						g_rightCannon.bCanFire = 1;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
					else
					{
						g_rightCannon.bullets[loop].ffixPosY -= FIXED_BULLET_YVEL;
						if(g_rightCannon.bullets[loop].ffixPosY >= 0)
							SpriteSetPos(g_rightCannon.bullets[loop].nSpriteIndex, FIXED_INT_PART(g_rightCannon.bullets[loop].ffixPosX), FIXED_INT_PART(g_rightCannon.bullets[loop].ffixPosY));
					}
				}
			}

			// HERE!!!!!!! - fix non-negative sprite pos issue

		} // end update bullet positions

		// collision detection for balls against paddle, blocks, and screen edges.
		for(loop = 0; loop < MAX_ONSCREEN_BALL_SPRITES; loop++)
		{
			if(g_BallSprites[loop].nState == BALL_STATE_ALIVE)
			{
				// check collisions with balls and paddle
				if( (g_BallSprites[loop].ffixPosX > g_Paddles[g_nCurrentPaddleIndex].ffixPosX && g_BallSprites[loop].ffixPosX < g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth) ||
					(g_BallSprites[loop].ffixPosX + FIXED_BALL_WIDTH > g_Paddles[g_nCurrentPaddleIndex].ffixPosX && g_BallSprites[loop].ffixPosX + FIXED_BALL_WIDTH < g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth) )
				{
					if(g_BallSprites[loop].ffixPosY + FIXED_BALL_HEIGHT > g_Paddles[g_nCurrentPaddleIndex].ffixPosY && g_BallSprites[loop].ffixPosY + FIXED_BALL_HEIGHT < g_Paddles[g_nCurrentPaddleIndex].ffixPosY + g_nfixPaddleHeight)
					{
						// change ball's x and y velocity based on where it hits the paddle
						if(g_BallSprites[loop].ffixPosX + FIXED_3 < g_Paddles[g_nCurrentPaddleIndex].ffixPosX)
							ballHitPaddlePos = g_Paddles[g_nCurrentPaddleIndex].ffixPosX;
						else if (g_BallSprites[loop].ffixPosX + FIXED_3 > g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth - FIXED_1)
							ballHitPaddlePos = g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth - FIXED_1;
						else
							ballHitPaddlePos = g_BallSprites[loop].ffixPosX + FIXED_3;

						// get the new theta angle
						g_nPaddleMapIndex = FIXED_INT_PART(ballHitPaddlePos - g_Paddles[g_nCurrentPaddleIndex].ffixPosX);
						g_ffixTheta = g_Paddles[g_nCurrentPaddleIndex].paddle_map[(u8)g_nPaddleMapIndex];

						// calculate new x and y velocities
						g_ffixCVel = g_BallSprites[loop].ffixVelC;
						g_BallSprites[loop].ffixTheta = g_ffixTheta;
						CalcTrig_XY();

						// check new y vel constraint
						if(g_ffixYVel > INITIAL_BALL_YVEL_MAX)
						{
							// now that y vel has changed again, trig must be recalculated
							g_ffixYVel = INITIAL_BALL_YVEL_MAX;
							CalcTrig_CX();
							g_BallSprites[loop].ffixVelC = g_ffixCVel;
						}

						// assign the new velocities

						// since i'm not using angles > 90 degrees, if the ball hits on the left half of the paddle,
						// make sure x velocity is negative. (new x and y vel are always positive)
						if(g_nPaddleMapIndex < FIXED_INT_PART(g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth) >> 1)
							g_BallSprites[loop].ffixVelX = -g_ffixXVel;
						else
							g_BallSprites[loop].ffixVelX = g_ffixXVel;

						// make y vel negative so ball goes back up
						g_BallSprites[loop].ffixVelY = -g_ffixYVel;

						g_BallSprites[loop].ffixPosY -= FIXED_2; // make sure ball is away from paddle, so it doesn't get "stuck"

						// update score
						g_score += SCORE_PADDLE;
					}
				}

				// Check collisions with ball and blocks.
				// Depending on direction, use a point on the ball to see if it has hit a block

				// going right
				if(g_BallSprites[loop].ffixVelX > 0)
				{																												// use point on
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosX + FIXED_BALL_WIDTH), FIXED_8 ) );	// right side of ball,
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosY + FIXED_3), FIXED_8 ) );			// halfway down the ball

					blockTileIndex = (blockTileY << 5) + blockTileX;

					// find out which 8x8 pixel tile this actually is on the map
					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00)	// if true, we've hit a block on our right!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						g_BallSprites[loop].ffixVelX = -g_BallSprites[loop].ffixVelX;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
				}
				else // going left
				{																										// use point on
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( g_BallSprites[loop].ffixPosX, FIXED_8 ) );				// left side of ball,
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosY + FIXED_3), FIXED_8 ) );	// halfway down the ball

					blockTileIndex = (blockTileY << 5) + blockTileX;

					// find out which 8x8 pixel tile this actually is on the map
					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00)	// if true, we've hit a block on our left!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						g_BallSprites[loop].ffixVelX = -g_BallSprites[loop].ffixVelX;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
				}

				// going down
				if(g_BallSprites[loop].ffixVelY > 0)
				{																												// use point on
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosX + FIXED_3), FIXED_8 ) );			// bottom side of ball,
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosY + FIXED_BALL_HEIGHT), FIXED_8 ) );	// halfway in the x direction

					blockTileIndex = (blockTileY << 5) + blockTileX;

					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00) // if true, we've hit a block below us!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						g_BallSprites[loop].ffixVelY = -g_BallSprites[loop].ffixVelY;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
				}
				else // going up
				{																										// use point on
					blockTileX = FIXED_INT_PART( FIXED_DIVIDE( (g_BallSprites[loop].ffixPosX + FIXED_3), FIXED_8 ) );	// top side of ball,
					blockTileY = FIXED_INT_PART( FIXED_DIVIDE( g_BallSprites[loop].ffixPosY, FIXED_8 ) );				// halfway in the x direction

					blockTileIndex = (blockTileY << 5) + blockTileX;

					blockHalfTarget = pMem[blockTileIndex];
					if(blockHalfTarget != 0x00) // if true, we've hit a block above us!
					{
						// since we are only going by 8x8 pixel blocks, we have to find out which
						// other 8x8 block to delete, if return from modulus is even, it's to our right, else left
						nModulus = blockHalfTarget % 2;
						if(nModulus == 0)
							pMem[blockTileIndex+1] = 0x00;
						else
							pMem[blockTileIndex-1] = 0x00;

						pMem[blockTileIndex] = 0x00;

						// update score
						g_score += SCORE_BLOCK;

						g_NumBlocksRemaining--;

						g_BallSprites[loop].ffixVelY = -g_BallSprites[loop].ffixVelY;

						CheckCapsuleCreate(blockTileX, blockTileY, nModulus);
					}
				}

				// check collisions with ball vs edges of screen

				if(g_BallSprites[loop].ffixVelX > 0)
				{
					if(g_BallSprites[loop].ffixPosX + FIXED_BALL_WIDTH + g_BallSprites[loop].ffixVelX < FIXED_SCREEN_WIDTH)
						g_BallSprites[loop].ffixPosX += g_BallSprites[loop].ffixVelX;
					else
						g_BallSprites[loop].ffixVelX = -g_BallSprites[loop].ffixVelX;
				}
				else
				{
					if(g_BallSprites[loop].ffixPosX + g_BallSprites[loop].ffixVelX > 0)
						g_BallSprites[loop].ffixPosX += g_BallSprites[loop].ffixVelX;
					else
						g_BallSprites[loop].ffixVelX = -g_BallSprites[loop].ffixVelX;
				}

				if(g_BallSprites[loop].ffixVelY > 0)
				{
					if(g_BallSprites[loop].ffixPosY + g_BallSprites[loop].ffixVelY < FIXED_SCREEN_HEIGHT)
						g_BallSprites[loop].ffixPosY += g_BallSprites[loop].ffixVelY;
					else
					{
						// if this is the last ball left
						if(g_nBallCount == 1)
						{
							// player just died

							/*																// DEBUG
							DeleteAllCapsuleSprites();

							HideBallSprite(loop);

							fpGame = DeathSequence;
							return;
							*/																// DEBUG

							g_BallSprites[loop].ffixVelY = -g_BallSprites[loop].ffixVelY;		// DEBUG
						}
						else
						{
							// other balls still exist in the playing field,
							// so simply delete this ball and let the player keep playing.
							HideBallSprite(loop);
							continue;
						}
					}
				}
				else
				{
					if(g_BallSprites[loop].ffixPosY + g_BallSprites[loop].ffixVelY > 0)
						g_BallSprites[loop].ffixPosY += g_BallSprites[loop].ffixVelY;
					else
						g_BallSprites[loop].ffixVelY = -g_BallSprites[loop].ffixVelY;
				}

				SpriteSetPos(g_BallSprites[loop].nSpriteIndex, FIXED_INT_PART(g_BallSprites[loop].ffixPosX), FIXED_INT_PART(g_BallSprites[loop].ffixPosY));
			}
		}

		if(g_NumBlocksRemaining == 0)
		{
			// stop mod file
			MP_StopModuleArm();

			// TODO: play sound here
			

			PauseGame(2);

			LoadNextLevel();
		}

		g_bDraw = 0;
	}
}

void GameplayDelete()
{
	MP_PlayModuleArm(GetModFile(MOD_BLADERAVER), 1);

	DeleteAllBallSprites();
	DeleteAllSprites();
	
	InitBackgrounds();
	InitSpriteMemory();
	
	InitCapsuleSprites();

	g_lives = TOTAL_LIVES;
	g_score = 0;

	fpGame = LogoCreate;
}

void UpdateCapsules()
{
	for(u8 i = 0; i < MAX_CAPSULE_SPRITES_ON_SCREEN; i++)
	{
		if(g_CapsuleSprites[i].nSpriteState == SPRITE_STATE_ALIVE)
		{
			// check if capsule is past end of screen
			if(g_CapsuleSprites[i].ffixPosY > FIXED_SCREEN_HEIGHT)
			{
				// destroy the capsule
				DeleteCapsuleSprite(i);
			}
			// check collision with paddle
			else if((g_CapsuleSprites[i].ffixPosY > g_Paddles[g_nCurrentPaddleIndex].ffixPosY && g_CapsuleSprites[i].ffixPosY < g_Paddles[g_nCurrentPaddleIndex].ffixPosY + g_nfixPaddleHeight) ||
					(g_CapsuleSprites[i].ffixPosY + FIXED_CAPSULE_HEIGHT > g_Paddles[g_nCurrentPaddleIndex].ffixPosY && g_CapsuleSprites[i].ffixPosY + FIXED_CAPSULE_HEIGHT < g_Paddles[g_nCurrentPaddleIndex].ffixPosY + g_nfixPaddleHeight))
			{
				if((g_CapsuleSprites[i].ffixPosX > g_Paddles[g_nCurrentPaddleIndex].ffixPosX && g_CapsuleSprites[i].ffixPosX < g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth) ||
				   (g_CapsuleSprites[i].ffixPosX + FIXED_CAPSULE_WIDTH > g_Paddles[g_nCurrentPaddleIndex].ffixPosX && g_CapsuleSprites[i].ffixPosX + FIXED_CAPSULE_WIDTH < g_Paddles[g_nCurrentPaddleIndex].ffixPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth))
				{
					// perform powerup
					fp fpOnPowerup = fpCapsuleTable[g_CapsuleSprites[i].nCapsuleType];
					fpOnPowerup();

					// destroy the capsule
					DeleteCapsuleSprite(i);
				}
				else
				{
					// update capsule pos
					g_CapsuleSprites[i].ffixPosY += FIXED_CAPSULE_Y_VEL;
					SpriteSetPos(g_CapsuleSprites[i].nSpriteIndex, FIXED_INT_PART(g_CapsuleSprites[i].ffixPosX), FIXED_INT_PART(g_CapsuleSprites[i].ffixPosY));
				}
			}
			else
			{
				// update capsule pos
				g_CapsuleSprites[i].ffixPosY += FIXED_CAPSULE_Y_VEL;
				SpriteSetPos(g_CapsuleSprites[i].nSpriteIndex, FIXED_INT_PART(g_CapsuleSprites[i].ffixPosX), FIXED_INT_PART(g_CapsuleSprites[i].ffixPosY));
			}
		}
	}
}

// If paddle doesn't already have cannons, attaches cannon sprites to paddle
// and allows player to use L and R buttons to shoot.
void OnPowerupCapsuleCannon()
{
	if(g_bHasCannon == 0)
	{
		// move cannon sprites to ends of paddle
		AttachCannons();

		// enable cannons
		g_bHasCannon = 1;
	}
}

// Adds two more balls. Each ball has slightly different X vel than the original ball,
// so a new theta will be calculated for each new ball. Each ball has same Y vel and same C vel as original ball.
void OnPowerupCapsuleMultiball()
{
	FIXED ffixNewTheta;

	for(loop = 0; loop < MAX_ONSCREEN_BALL_SPRITES; loop++)
	{
		if(g_BallSprites[loop].nState == BALL_STATE_ALIVE)
		{
			// one of the balls will have less of an angle than the original ball
			ffixNewTheta = g_BallSprites[loop].ffixTheta - FIXED_THETA_MULTIBALL_DELTA;
			
			// check constraints of new theta
			if(ffixNewTheta < BALL_THETA_MIN)
				ffixNewTheta = BALL_THETA_MIN;
			
			// Keep same C vel as original ball, but recalculate new X and Y vel
			g_ffixTheta = ffixNewTheta;
			g_ffixCVel  = g_BallSprites[loop].ffixVelC;
			CalcTrig_XY();

			// make sure X vel and Y vel are going in correct direction
			if(g_BallSprites[loop].ffixVelX < 0)
				g_ffixXVel = -g_ffixXVel;
			if(g_BallSprites[loop].ffixVelY < 0)
				g_ffixYVel = -g_ffixYVel;

			AddBallSprite(g_BallSprites[loop].ffixPosX, g_BallSprites[loop].ffixPosY, g_ffixXVel, g_ffixYVel, g_ffixCVel, g_ffixTheta);

			// the other ball will have more of an angle than the original ball
			ffixNewTheta = g_BallSprites[loop].ffixTheta + FIXED_THETA_MULTIBALL_DELTA;

			// check constraints of new theta
			if(ffixNewTheta > BALL_THETA_MAX)
				ffixNewTheta = BALL_THETA_MAX;

			// Keep same C vel as original ball, but recalculate new X and Y vel
			g_ffixTheta = ffixNewTheta;
			CalcTrig_XY();

			// make sure X vel and Y vel are going in correct direction
			if(g_BallSprites[loop].ffixVelX < 0)
				g_ffixXVel = -g_ffixXVel;
			if(g_BallSprites[loop].ffixVelY < 0)
				g_ffixYVel = -g_ffixYVel;

			AddBallSprite(g_BallSprites[loop].ffixPosX, g_BallSprites[loop].ffixPosY, g_ffixXVel, g_ffixYVel, g_ffixCVel, g_ffixTheta);
		}
	}

	AddCreatedBallSprites();
}

// makes paddle grow up a size
void OnPowerupCapsuleGrow()
{
	if(g_nCurrentPaddleIndex < 2)
	{
		g_nLastPaddleIndex = g_nCurrentPaddleIndex;
		g_nCurrentPaddleIndex++;

		u8 nUnused1, nUnused2;
		FIXED ffixNewPosX;

		// determine unused paddle indexes
		if(g_nLastPaddleIndex == 0)
		{
			nUnused1 = 0;
			nUnused2 = 2;
		}
		else
		{
			nUnused1 = 0;
			nUnused2 = 1;
		}

		// set pos of bigger paddle
		g_Paddles[g_nCurrentPaddleIndex].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;

		// middle of new paddle should match up with middle of old paddle
		ffixNewPosX = g_Paddles[g_nLastPaddleIndex].ffixPosX + FIXED_DIVIDE(g_Paddles[g_nLastPaddleIndex].nfixPaddleWidth, FIXED_2) - FIXED_DIVIDE(g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth, FIXED_2);

		// check constraints of new x pos vs sides of screen
		if(ffixNewPosX < 0)
			ffixNewPosX = 0;
		else if(ffixNewPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth >= FIXED_SCREEN_WIDTH)
			ffixNewPosX = FIXED_SCREEN_WIDTH - FIXED_1 - g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth;

		g_Paddles[g_nCurrentPaddleIndex].ffixPosX = ffixNewPosX;

		MovePaddle(g_nCurrentPaddleIndex, g_Paddles[g_nCurrentPaddleIndex].ffixPosX, g_Paddles[g_nCurrentPaddleIndex].ffixPosY);
		g_Paddles[g_nCurrentPaddleIndex].nState = PADDLE_STATE_ALIVE;

		// set unused paddles offscreen and to null state
		g_Paddles[nUnused1].ffixPosX = FIXED_SCREEN_WIDTH;
		g_Paddles[nUnused2].ffixPosX = FIXED_SCREEN_WIDTH;
		g_Paddles[nUnused1].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;
		g_Paddles[nUnused2].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;
		MovePaddle(nUnused1, FIXED_SCREEN_WIDTH, g_Paddles[g_nLastPaddleIndex].ffixPosY);
		MovePaddle(nUnused2, FIXED_SCREEN_WIDTH, g_Paddles[g_nLastPaddleIndex].ffixPosY);
		g_Paddles[nUnused1].nState = PADDLE_STATE_NULL;
		g_Paddles[nUnused2].nState = PADDLE_STATE_NULL;
	}
}

// makes paddle shrink down a size
void OnPowerupCapsuleShrink()
{
	if(g_nCurrentPaddleIndex > 0)
	{
		g_nLastPaddleIndex = g_nCurrentPaddleIndex;
		g_nCurrentPaddleIndex--;

		u8 nUnused1, nUnused2;
		FIXED ffixNewPosX;

		// determine unused paddle indexes
		if(g_nLastPaddleIndex == 2)
		{
			nUnused1 = 0;
			nUnused2 = 2;
		}
		else
		{
			nUnused1 = 1;
			nUnused2 = 2;
		}

		// set pos of smaller paddle
		g_Paddles[g_nCurrentPaddleIndex].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;

		// middle of new paddle should match up with middle of old paddle
		ffixNewPosX = g_Paddles[g_nLastPaddleIndex].ffixPosX + FIXED_DIVIDE(g_Paddles[g_nLastPaddleIndex].nfixPaddleWidth, FIXED_2) - FIXED_DIVIDE(g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth, FIXED_2);

		/*
		// check constraints of new x pos vs sides of screen
		if(ffixNewPosX < 0)
			ffixNewPosX = 0;
		else if(ffixNewPosX + g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth >= FIXED_SCREEN_WIDTH)
			ffixNewPosX = FIXED_SCREEN_WIDTH - FIXED_1 - g_Paddles[g_nCurrentPaddleIndex].nfixPaddleWidth;
		*/

		g_Paddles[g_nCurrentPaddleIndex].ffixPosX = ffixNewPosX;

		MovePaddle(g_nCurrentPaddleIndex, g_Paddles[g_nCurrentPaddleIndex].ffixPosX, g_Paddles[g_nCurrentPaddleIndex].ffixPosY);
		g_Paddles[g_nCurrentPaddleIndex].nState = PADDLE_STATE_ALIVE;

		// set unused paddles offscreen and to null state
		g_Paddles[nUnused1].ffixPosX = FIXED_SCREEN_WIDTH;
		g_Paddles[nUnused2].ffixPosX = FIXED_SCREEN_WIDTH;
		g_Paddles[nUnused1].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;
		g_Paddles[nUnused2].ffixPosY = g_Paddles[g_nLastPaddleIndex].ffixPosY;
		MovePaddle(nUnused1, FIXED_SCREEN_WIDTH, g_Paddles[g_nLastPaddleIndex].ffixPosY);
		MovePaddle(nUnused2, FIXED_SCREEN_WIDTH, g_Paddles[g_nLastPaddleIndex].ffixPosY);
		g_Paddles[nUnused1].nState = PADDLE_STATE_NULL;
		g_Paddles[nUnused2].nState = PADDLE_STATE_NULL;
	}
}

// slows down all balls, maintaining angles
// and keeps within constraints of trig system
void OnPowerupCapsuleSlowball()
{
	FIXED ffixNewVelY;

	for(loop = 0; loop < MAX_ONSCREEN_BALL_SPRITES; loop++)
	{
		if(g_BallSprites[loop].nState == BALL_STATE_ALIVE)
		{
			// going down
			if(g_BallSprites[loop].ffixVelY > 0)
			{
				ffixNewVelY = g_BallSprites[loop].ffixVelY - POWERUP_BALL_Y_VEL_FACTOR;

				// check constraints of new velocity
				if(ffixNewVelY < INITIAL_BALL_YVEL_MIN)
					ffixNewVelY = INITIAL_BALL_YVEL_MIN;

				g_ffixYVel = ffixNewVelY;
			}
			else // going up
			{
				ffixNewVelY = g_BallSprites[loop].ffixVelY + POWERUP_BALL_Y_VEL_FACTOR;

				// check constraints of new velocity
				if(ffixNewVelY > -INITIAL_BALL_YVEL_MIN)
					ffixNewVelY = -INITIAL_BALL_YVEL_MIN;

				g_ffixYVel = -ffixNewVelY;
			}

			g_BallSprites[loop].ffixVelY = ffixNewVelY;
			g_ffixTheta = g_BallSprites[loop].ffixTheta;
			CalcTrig_CX();

			// now check constraints of new X vel
			if(g_ffixXVel < INITIAL_BALL_XVEL_MIN)
			{
				g_ffixXVel = INITIAL_BALL_XVEL_MIN;

				// now that X vel has changed, recalculate trig
				CalcTrig_CY();

				if(g_BallSprites[loop].ffixVelY < 0) // going up
					g_BallSprites[loop].ffixVelY = -g_ffixYVel;
				else // going down
					g_BallSprites[loop].ffixVelY = g_ffixYVel;
			}

			g_BallSprites[loop].ffixVelC = g_ffixCVel;
			
			// going right
			if(g_BallSprites[loop].ffixVelX > 0)
				g_BallSprites[loop].ffixVelX = g_ffixXVel;
			else // going left
				g_BallSprites[loop].ffixVelX = -g_ffixXVel;
		}
	}
}

// speeds up all balls, maintaining angles
// and keeps within constraints of trig system
void OnPowerupCapsuleFastball()
{
	FIXED ffixNewVelY;

	for(loop = 0; loop < MAX_ONSCREEN_BALL_SPRITES; loop++)
	{
		if(g_BallSprites[loop].nState == BALL_STATE_ALIVE)
		{
			// going down
			if(g_BallSprites[loop].ffixVelY > 0)
			{
				ffixNewVelY = g_BallSprites[loop].ffixVelY + POWERUP_BALL_Y_VEL_FACTOR;

				// check constraints of new velocity
				if(ffixNewVelY > INITIAL_BALL_YVEL_MAX)
					ffixNewVelY = INITIAL_BALL_YVEL_MAX;

				g_ffixYVel = ffixNewVelY;
			}
			else // going up
			{
				ffixNewVelY = g_BallSprites[loop].ffixVelY - POWERUP_BALL_Y_VEL_FACTOR;

				// check constraints of new velocity
				if(ffixNewVelY < -INITIAL_BALL_YVEL_MAX)
					ffixNewVelY = -INITIAL_BALL_YVEL_MAX;

				g_ffixYVel = -ffixNewVelY;
			}

			g_BallSprites[loop].ffixVelY = ffixNewVelY;
			g_ffixTheta = g_BallSprites[loop].ffixTheta;
			CalcTrig_CX();

			// now check constraints of new X vel
			if(g_ffixXVel > INITIAL_BALL_XVEL_MAX)
			{
				g_ffixXVel = INITIAL_BALL_XVEL_MAX;

				// now that X vel has changed, recalculate trig
				CalcTrig_CY();

				if(g_BallSprites[loop].ffixVelY < 0) // going up
					g_BallSprites[loop].ffixVelY = -g_ffixYVel;
				else // going down
					g_BallSprites[loop].ffixVelY = g_ffixYVel;
			}

			g_BallSprites[loop].ffixVelC = g_ffixCVel;
			
			// going right
			if(g_BallSprites[loop].ffixVelX > 0)
				g_BallSprites[loop].ffixVelX = g_ffixXVel;
			else // going left
				g_BallSprites[loop].ffixVelX = -g_ffixXVel;
		}
	}
}

void ScrollBackground()
{
	// animate the background
	if(g_levels[g_currentLevel].bAnimatedBG)
	{
		// is it time to advance a frame?
		if(++g_animationTicks == g_levels[g_currentLevel].nAnimationSpeed)
		{
			pMem = (u16*)ScreenBaseBlock(31);

			// is animation "ping-pong" style?
			if(g_levels[g_currentLevel].bAnimationPingPong)
			{
				// if on last frame of animation, switch direction
				if(pMem[0] == g_levels[g_currentLevel].nBGMapIndexEnd)
				{
					g_animationDirection = -1;
				}
				// else if on first frame of animation, switch direction
				else if(pMem[0] == g_levels[g_currentLevel].nBGMapIndexStart)
				{
					g_animationDirection = 1;
				}

				BGFillMap(31, pMem[0] + g_animationDirection);
			}
			// else animation is looped
			else
			{
				// if on last frame of animation, go back to first
				if(pMem[0] == g_levels[g_currentLevel].nBGMapIndexEnd)
					BGFillMap(31, g_levels[g_currentLevel].nBGMapIndexStart);
				else
					BGFillMap(31, pMem[0] + 1);
			}

			g_animationTicks = 0;
		}
	}

	// update scrolling bg
	g_ffixBgXOffset += g_fxhvel;
	g_ffixBgYOffset += g_fxvvel;

	if(FIXED_INT_PART(g_ffixBgXOffset) > 0)
		REG_BG2HOFS = (u16)(512-FIXED_INT_PART(g_ffixBgXOffset));	// this actually scrolls to the left (move image over to the right)
	else
		REG_BG2HOFS = (u16)FIXED_INT_PART(g_ffixBgXOffset);			// while this scrolls to the right (moves image to the left)

	if(FIXED_INT_PART(g_ffixBgYOffset) > 0)
		REG_BG2VOFS = (u16)(255-FIXED_INT_PART(g_ffixBgYOffset));	// this actually scrolls up (moves image down)
	else
		REG_BG2VOFS = (u16)FIXED_INT_PART(g_ffixBgYOffset);			// while this scrolls down (moves image up)
}

// uses random number to see if a capsule is created when the ball hits a block
void CheckCapsuleCreate(u16 nBlockTileX, u16 nBlockTileY, u8 nModulus)
{
	u8 nCapsuleType;
	//s16 nNewCapsuleSpriteIndex;
	s16 nNewCapsuleX;
	s16 nNewCapsuleY;
	s16 nNewCapsuleIndex;

	// get index of randomly dropped capsule
	nCapsuleType = g_nCapsuleTable[g_nRand8];
	if (nCapsuleType != CAPSULE_NULL)
	{
		// check to see if there is room for another capsule sprite
		nNewCapsuleIndex = CheckForAvailableCapsuleIndex();

		if(nNewCapsuleIndex != -1)
		{
			// calculate pos of new capsule
			if(nModulus == 0)
				nNewCapsuleX = nBlockTileX << 3;
			else
				nNewCapsuleX = (nBlockTileX-1) << 3;

			nNewCapsuleY = nBlockTileY << 3;

			nNewCapsuleSpriteIndex = SpriteCreate(g_nGameplayCharNums[nCapsuleType], nNewCapsuleX,
										nNewCapsuleY, 0, 0, 1, SPRITE_SIZE_8, SPRITE_SHAPE_WIDE, SPRITE_COLOR_256);

			if(nNewCapsuleSpriteIndex != -1)
			{
				// stores "copy" of sprite data
				CreateCapsuleSprite(nCapsuleType, nNewCapsuleSpriteIndex, nNewCapsuleIndex, nNewCapsuleX, nNewCapsuleY);
			}
		}
	}
}

// fade level in using alpha blend
void FadeInLevel()
{
	g_ffixTransparencyFactor = FLOAT_TO_FIXED(0);	// 0 to 16
	g_ffixTarget2AlphaFactor = FLOAT_TO_FIXED(16);	// 16 to 0
	REG_COLEV = (FIXED_INT_PART(g_ffixTarget2AlphaFactor)<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);	// transparency factor (can be 0-16)
	REG_BLDMOD = TARGET1_BG1 | TARGET2_BG2 | MODE_NORMAL;

	while(g_ffixTransparencyFactor < FLOAT_TO_FIXED(16) || g_ffixTarget2AlphaFactor > 0)
	{
		if(g_bDraw == 1)
		{
			ScrollBackground();

			if(g_ffixTransparencyFactor + g_ffixBGAlphaBlendFactor <= FLOAT_TO_FIXED(16))
				g_ffixTransparencyFactor += g_ffixBGAlphaBlendFactor;
			else
				g_ffixTransparencyFactor = FLOAT_TO_FIXED(16);
			if(g_ffixTarget2AlphaFactor - g_ffixBGAlphaBlendFactor >= 0)
				g_ffixTarget2AlphaFactor -= g_ffixBGAlphaBlendFactor;
			else
				g_ffixTarget2AlphaFactor = 0;

			REG_COLEV = (FIXED_INT_PART(g_ffixTarget2AlphaFactor)<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);

			g_bDraw = 0;
		}
	}

	// end of alpha blend
	REG_BLDMOD = 0;
}

void LoadFirstLevel()
{
	g_currentLevel = 0;
	currentModFile = 0;

	// load level and it's background
	LoadLevelData();

	// draw player stat info
	TextClear(29);
	DrawStats();

	// reset animation timer and direction
	g_animationTicks = 0;
	g_animationDirection = 1;

	// set number of blocks remaining for current level;
	g_NumBlocksRemaining = g_levels[g_currentLevel].nBlocks;

	ResetBallAndPaddlePos();

	// fade the level blocks in
	FadeInLevel();

	GameplayCountdown();
}

void LoadNextLevel()
{
	DeleteAllCapsuleSprites();

	// check if player just beat the last level
	if(g_currentLevel+2 > NUM_LEVELS)
	{
		// stop mod file
		MP_StopModuleArm();

		// play first mod
		MP_PlayModuleArm(GetModFile(0), 1);

		// loop to first level
		LoadFirstLevel();
	}
	else
	{
		// update score
		g_score += SCORE_LEVEL(g_currentLevel);

		// advance to next level
		g_currentLevel++;

		// load level and it's background
		LoadLevelData();

		// draw player stat info
		TextClear(29);
		DrawStats();

		// reset animation timer and direction
		g_animationTicks = 0;
		g_animationDirection = 1;

		// set number of blocks remaining for new level;
		g_NumBlocksRemaining = g_levels[g_currentLevel].nBlocks;

		ResetBallAndPaddlePos();

		// fade the level blocks in
		FadeInLevel();

		// advance mod file
		if(currentModFile + 1 == NUM_MOD_FILES)
			currentModFile = 0;
		else
			currentModFile++;

		MP_PlayModuleArm(GetModFile(currentModFile), 1);

		GameplayCountdown();
	}
}

void LoadLevelData()
{
	// load level map
	BGLoadMap(30, g_levels[g_currentLevel].pMap);

	// if level uses a map for it's bg, load it.
	if(g_levels[g_currentLevel].bUseBGMapArray)
	{
		BGLoadMap(31, g_levels[g_currentLevel].pBGMap);
	}
	// else fill the bg with the start index
	else
	{
		BGFillMap(31, g_levels[g_currentLevel].nBGMapIndexStart);
	}
}

// resets mid sized paddle in center of screen and resets ball just above paddle.
void ResetBallAndPaddlePos()
{
	FIXED nfixPosY = INT_TO_FIXED(INITIAL_PADDLE_Y_POS);

	// set unused paddles offscreen and to null state
	g_Paddles[0].ffixPosX = FIXED_SCREEN_WIDTH;
	g_Paddles[2].ffixPosX = FIXED_SCREEN_WIDTH;
	g_Paddles[0].ffixPosY = nfixPosY;
	g_Paddles[2].ffixPosY = nfixPosY;
	MovePaddle(0, FIXED_SCREEN_WIDTH, nfixPosY);
	MovePaddle(2, FIXED_SCREEN_WIDTH, nfixPosY);
	g_Paddles[0].nState = PADDLE_STATE_NULL;
	g_Paddles[2].nState = PADDLE_STATE_NULL;

	// set pos of mid sized paddle
	g_Paddles[1].ffixPosX = FLOAT_TO_FIXED((SCREEN_WIDTH/2) - (FIXED_INT_PART(g_Paddles[1].nfixPaddleWidth)/2));
	g_Paddles[1].ffixPosY = nfixPosY;
	MovePaddle(1, g_Paddles[1].ffixPosX, g_Paddles[1].ffixPosY);
	g_Paddles[1].nState = PADDLE_STATE_ALIVE;
	g_nCurrentPaddleIndex = 1; // set current paddle to mid sized paddle

	// reset ball pos and vel
	InitBallSprites();	// might need to be moved elsewhere
	AddBallSprite(g_Paddles[1].ffixPosX, INT_TO_FIXED(120), INITIAL_BALL_X_VEL, INITIAL_BALL_Y_VEL, FIXED_INITIAL_BALL_C_VEL, FIXED_INITIAL_BALL_THETA);
	AddCreatedBallSprites();
}

void DrawStats()
{
	char szScore[12];
	char szCurrentLevel[3];
	char szLives[2];

	// make sure score has room to fit on the screen
	if(g_score > 99999)
		g_score = 0;
	sprintf(szScore, "SCORE %d", g_score);
	TextToBG(29, 1,  18, szScore);

	sprintf(szCurrentLevel, "LEVEL %d", g_currentLevel+1);
	TextToBG(29, 13, 18, szCurrentLevel);

	sprintf(szLives, "LIVES %d", g_lives);
	TextToBG(29, 22, 18, szLives);

	//DEBUG
	char szXPos[10];
	sprintf(szXPos, "X-%d", FIXED_INT_PART(g_leftCannon.bullets[0].ffixPosX));
	TextToBG(29, 1,  17, szXPos);

	char szYPos[10];
	sprintf(szYPos, "Y-%d", FIXED_INT_PART(g_leftCannon.bullets[0].ffixPosY));
	TextToBG(29, 1,  16, szYPos);

	char szIndex[10];
	sprintf(szIndex, "I-%d", g_leftCannon.bullets[0].nSpriteIndex);
	TextToBG(29, 1,  15, szIndex);

	/*
	// DEBUG TRIG
	char szTheta[3];
	sprintf(szTheta, "%d", FIXED_INT_PART(g_ffixTheta));
	TextToBG(29, 1,  17, szTheta);

	char szXVel[10];
	s32 xvel = FIXED_INT_PART(g_BallSprites[0].ffixVelX);
	s32 xveldec = 100*(unsigned int)(g_BallSprites[0].ffixVelX & 0x000000ff)/256;
	sprintf(szXVel, "X %d.%d", xvel, xveldec);
	TextToBG(29, 1,  15, szXVel);

	char szYVel[10];
	s32 yvel = FIXED_INT_PART(g_BallSprites[0].ffixVelY);
	s32 yveldec = 100*(unsigned int)(g_BallSprites[0].ffixVelY & 0x000000ff)/256;
	sprintf(szYVel, "Y %d.%d", yvel, yveldec);
	TextToBG(29, 1,  16, szYVel);

	char szPowYFactor[10];
	s32 powy = FIXED_INT_PART(POWERUP_BALL_Y_VEL_FACTOR);
	s32 powydec = 100*(unsigned int)(POWERUP_BALL_Y_VEL_FACTOR & 0x000000ff)/256;
	sprintf(szPowYFactor, "P %d.%d", powy, powydec);
	TextToBG(29, 1,  13, szPowYFactor);

	char szCVel[10];
	s32 cvel = FIXED_INT_PART(g_BallSprites[0].ffixVelC);
	s32 cveldec = 100*(unsigned int)(g_BallSprites[0].ffixVelC & 0x000000ff)/256;
	sprintf(szCVel, "C %d.%d", cvel, cveldec);
	TextToBG(29, 1,  14, szCVel);

	char szMVel[12];
	s32 mvel = FIXED_INT_PART(INITIAL_BALL_XVEL_MAX);
	s32 mveldec = 100*(unsigned int)(INITIAL_BALL_XVEL_MAX & 0x000000ff)/256;
	sprintf(szMVel, "MAX %d.%d", mvel, mveldec);
	TextToBG(29, 1,  12, szMVel);

	char szMIVel[12];
	s32 mivel = FIXED_INT_PART(INITIAL_BALL_XVEL_MIN);
	s32 miveldec = 100*(unsigned int)(INITIAL_BALL_XVEL_MIN & 0x000000ff)/256;
	sprintf(szMIVel, "MIN %d.%d", mivel, miveldec);
	TextToBG(29, 1,  11, szMIVel);
	*/
}

void DeathSequence()
{
	/*
		All balls have already been deleted at this point,
		so move paddle sprite down gradually while doing an alpha blend with the background.
		When done moving, pause for some time, then hide the dead paddle sprite.
	*/
	u8 i;

	if(g_bDraw == 1)
	{
		// move paddle sprite down while performing alpha blend
		if(INT_TO_FIXED(INITIAL_PADDLE_Y_POS) == g_Paddles[g_nCurrentPaddleIndex].ffixPosY)
		{
			// move paddle down slightly
			g_Paddles[g_nCurrentPaddleIndex].ffixPosY += g_ffixDeathPaddleYFactor;
			MovePaddle(g_nCurrentPaddleIndex, g_Paddles[g_nCurrentPaddleIndex].ffixPosX, g_Paddles[g_nCurrentPaddleIndex].ffixPosY);

			// setup alpha blend for dying paddle
			SpriteSetAlphaMode(g_nSpriteIndexEndSection[0], ATR0_ALPHA_MODE_TRANSPARANT);
			SpriteSetAlphaMode(g_nSpriteIndexEndSection[1], ATR0_ALPHA_MODE_TRANSPARANT);
			for(i = 0; i < g_Paddles[g_nCurrentPaddleIndex].nPaddleSection; i++)
				SpriteSetAlphaMode(g_Paddles[g_nCurrentPaddleIndex].nSpriteIndexMidSection[i], ATR0_ALPHA_MODE_TRANSPARANT);
			g_ffixTransparencyFactor = FLOAT_TO_FIXED(16);
			g_ffixTarget2AlphaFactor = FLOAT_TO_FIXED(8);
			REG_COLEV = (FIXED_INT_PART(g_ffixTarget2AlphaFactor)<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);	// transparency factor (can be 0-16)
			REG_BLDMOD = TARGET1_OBJPLAIN | TARGET2_BG2 | MODE_NOALPHA;
		}
		else
		{
			// continue to move paddle down until its off the screen
			if(g_Paddles[g_nCurrentPaddleIndex].ffixPosY < FIXED_SCREEN_HEIGHT)
			{
				// move paddle down slightly
				g_Paddles[g_nCurrentPaddleIndex].ffixPosY += g_ffixDeathPaddleYFactor;
				MovePaddle(g_nCurrentPaddleIndex, g_Paddles[g_nCurrentPaddleIndex].ffixPosX, g_Paddles[g_nCurrentPaddleIndex].ffixPosY);

				// gradually alpha blend the paddle to give appearence of fading into the background
				if(g_ffixTransparencyFactor - g_ffixPaddleAlphaBlendFactor >= 0)
					g_ffixTransparencyFactor -= g_ffixPaddleAlphaBlendFactor;
				else
					g_ffixTransparencyFactor = 0;
				if(g_ffixTarget2AlphaFactor + g_ffixPaddleAlphaBlendFactor <= FLOAT_TO_FIXED(16))
					g_ffixTarget2AlphaFactor += g_ffixPaddleAlphaBlendFactor;
				else
					g_ffixTarget2AlphaFactor = FLOAT_TO_FIXED(16);

				REG_COLEV = (FIXED_INT_PART(g_ffixTarget2AlphaFactor)<<8) + FIXED_INT_PART(g_ffixTransparencyFactor);
			}
			else
			{
				// check if lives are left and reset positions
				if(g_lives > 0)
				{
					PauseGame(1);

					g_lives--;

					// disable transparency
					REG_BLDMOD = 0;
					SpriteSetAlphaMode(g_nSpriteIndexEndSection[0], ATR0_ALPHA_MODE_NORMAL);
					SpriteSetAlphaMode(g_nSpriteIndexEndSection[1], ATR0_ALPHA_MODE_NORMAL);
					for(i = 0; i < g_Paddles[g_nCurrentPaddleIndex].nPaddleSection; i++)
						SpriteSetAlphaMode(g_Paddles[g_nCurrentPaddleIndex].nSpriteIndexMidSection[i], ATR0_ALPHA_MODE_NORMAL);

					// disable cannon powerup
					g_bHasCannon = 0;

					ResetBallAndPaddlePos();
					GameplayCountdown();
					fpGame = GameplayLoop;
					return;
				}
				else
				{
					// display gameover
					GameOver();
					return;
				}
			}
		}

		g_bDraw = 0;
	}
}

// displays "GAME OVER" in center screen, pauses for a few secs,
// deletes gameplay objects / memory, then resets to company logo screen.
void GameOver()
{
	TextToBG(29, 11, 9, "GAME OVER\0");

	// stop mod file
	MP_StopModuleArm();

	// pause game for a few secs
	PauseGame(3);

	// delete gameplay objects / memory and reset to company logo screen
	GameplayDelete();
}

void PauseGame(int nSeconds)
{
	g_tick = nSeconds;
	g_timeRet = g_lastTick = REG_TM3D = 1;

	// enable timers to overflow every second (using TIME_FREQUENCY_256)
	REG_TM2CNT = TIME_FREQUENCY_256 | TIMER_ENABLED;
	REG_TM3CNT = TIME_OVERFLOW | TIMER_ENABLED;

	while(g_tick > 0)
	{
		g_timeRet = REG_TM3D;

		// if timer overflowed, a second has passed
		if(g_timeRet - g_lastTick > 0)
		{
			g_tick--;
			g_timeRet = g_lastTick = REG_TM3D = 1;
		}

		if(g_lastTick != g_timeRet)
			g_lastTick = g_timeRet;
	}

	REG_TM2CNT = 0;
	REG_TM3CNT = 0;
}
