#include "../gba/gba.h"
#include "../gba/sprite.h"
#include "cloud.h"
#include "cloud01_01.h"
#include "cloud01_02.h"
#include "cloud01_03.h"

u16 g_nCloudCharNums[3];

void LoadCloudImages()
{
	// load cloud image data
	g_nCloudCharNums[0] = SpriteLoadImage(cloud01_01Data, cloud01_01_WIDTH, cloud01_01_HEIGHT, 256);
	g_nCloudCharNums[1] = SpriteLoadImage(cloud01_02Data, cloud01_02_WIDTH, cloud01_02_HEIGHT, 256);
	g_nCloudCharNums[2] = SpriteLoadImage(cloud01_03Data, cloud01_03_WIDTH, cloud01_03_HEIGHT, 256);
}

void Cloud::Init()
{
	//m_nFirstSpriteIndex = -1;
	m_nCloudSpriteIndexes[0] = -1;
	m_nCloudSpriteIndexes[1] = -1;
	m_nCloudSpriteIndexes[2] = -1;

	m_nFirstSpriteX = 0;
	m_nFirstSpriteY = 0;

	m_ffixGradualPosX = 0;
}

void Cloud::Create(s16 nPosX, s16 nPosY)
{
	// create cloud sprites
	m_nCloudSpriteIndexes[0] = SpriteCreate(g_nCloudCharNums[0], nPosX, nPosY, 0, 0, 1, SPRITE_SIZE_32, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	m_nCloudSpriteIndexes[1] = SpriteCreate(g_nCloudCharNums[1], nPosX+32, nPosY, 0, 0, 1, SPRITE_SIZE_32, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);
	m_nCloudSpriteIndexes[2] = SpriteCreate(g_nCloudCharNums[2], nPosX+64, nPosY, 0, 0, 1, SPRITE_SIZE_32, SPRITE_SHAPE_SQUARE, SPRITE_COLOR_256);

	// store position of first cloud sprite
	m_nFirstSpriteX = nPosX;
	m_nFirstSpriteY = nPosY;

	// store copy of pos as fixed float
	// (these position vars we will used to actaully move the cloud)
	m_ffixGradualPosX = INT_TO_FIXED(nPosX);
}

void Cloud::SetPos(s16 nPosX, s16 nPosY)
{
	SpriteSetPos(m_nCloudSpriteIndexes[0], nPosX, nPosY);
	SpriteSetPos(m_nCloudSpriteIndexes[1], nPosX+32, nPosY);
	SpriteSetPos(m_nCloudSpriteIndexes[2], nPosX+64, nPosY);

	m_nFirstSpriteX = nPosX;
	m_nFirstSpriteY = nPosY;
}

void Cloud::SetVel(float fVelX, float fVelY)
{
	m_fVelX = fVelX;
	m_fVelY = fVelY;
}

// uses current vel and pos to calculate new pos.
// moves the cloud until it reaches edge of screen then starts over from left side after wrap around
void Cloud::Update()
{
	m_ffixGradualPosX += FLOAT_TO_FIXED(m_fVelX);

	// wrap around
	if(FIXED_INT_PART(m_ffixGradualPosX) >= SCREEN_WIDTH)
	{
		SetPos(512-CLOUD_WIDTH, m_nFirstSpriteY);
		m_ffixGradualPosX = INT_TO_FIXED(-CLOUD_WIDTH);
	}
	else
	{
		if(m_ffixGradualPosX < 0)
			SetPos(FIXED_INT_PART(INT_TO_FIXED(512)+m_ffixGradualPosX), m_nFirstSpriteY);
		else
			SetPos(FIXED_INT_PART(m_ffixGradualPosX), m_nFirstSpriteY);
	}
}

void Cloud::Delete()
{
	// delete cloud sprites
	DeleteSprite(m_nCloudSpriteIndexes[0]);
	DeleteSprite(m_nCloudSpriteIndexes[1]);
	DeleteSprite(m_nCloudSpriteIndexes[2]);
}
