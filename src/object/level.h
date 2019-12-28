#ifndef __LEVEL_H__
#define __LEVEL_H__

// level maps
#include "../map/map01.h"
#include "../map/map02.h"
#include "../map/map03.h"
#include "../map/map04.h"
#include "../map/map05.h"

// bg maps
#include "../map/scrolling_bg_map.h"
#include "../map/scrolling_bg_mapDkGreenHatch.h"

#define NUM_LEVELS	5

u8 g_currentLevel = 0;

typedef struct LEVEL
{
	const u8*	pMap;				// pointer to this level's map data
	u16			nBlocks;			// total number of valid blocks in level
	u8			bUseBGMapArray;		// should the bg array be used to create the bg? (1) or should the bg be filled with nBGMapIndexStart? (0)
	const u8*	pBGMap;				// pointer to this level's background map data, if using it.
	u8			bAnimatedBG;		// is the level's background animated?
	u8			bAnimationPingPong;	// is the animation ping pong (1) style? or looping (0) style?
	u8			nAnimationSpeed;	// 0 = very fast, 255 = very slow
	u8			nBGMapIndexStart;	// first animation index (is valid even if bg is not animated)
	u8			nBGMapIndexEnd;		// last animation index
} LEVEL;

LEVEL	g_levels[NUM_LEVELS];
u8		g_animationTicks = 0;
s8		g_animationDirection = 1;

u8 nLevelCounter = 0;

void SetupLevel(const u8* pMap, u16 nBlocks, u8 bUseBGMapArray, const u8* pBGMap, u8 bAnimatedBG,
				u8 bAnimationPingPong, u8 nAnimationSpeed, u8 nBGMapIndexStart, u8 nBGMapIndexEnd)
{
	if(nLevelCounter == NUM_LEVELS)
		return;

	g_levels[nLevelCounter].pMap				= pMap;
	g_levels[nLevelCounter].nBlocks				= nBlocks;
	g_levels[nLevelCounter].bUseBGMapArray		= bUseBGMapArray;
	g_levels[nLevelCounter].pBGMap				= pBGMap;
	g_levels[nLevelCounter].bAnimatedBG			= bAnimatedBG;
	g_levels[nLevelCounter].bAnimationPingPong	= bAnimationPingPong;
	g_levels[nLevelCounter].nAnimationSpeed		= nAnimationSpeed;
	g_levels[nLevelCounter].nBGMapIndexStart	= nBGMapIndexStart;
	g_levels[nLevelCounter].nBGMapIndexEnd		= nBGMapIndexEnd;

	nLevelCounter++;
}

void SetupLevels()
{
	SetupLevel(map01, 105, 1, scrolling_bg_map, 0, 0, 0, 0, 0);
	SetupLevel(map02, 80, 0, 0, 1, 1, 5, 40, 43);
	SetupLevel(map03, 53, 0, 0, 1, 1, 5, 40, 43);
	SetupLevel(map04, 55, 0, 0, 1, 1, 5, 40, 43);
	SetupLevel(map05, 56, 0, 0, 1, 1, 5, 40, 43);

	nLevelCounter = 0;
}

#endif
