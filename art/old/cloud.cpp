#include "../gba/gba.h"
#include "cloud.h"
#include "../gba/sprite.h"
#include "../art/cloud01_01.h"
#include "../art/cloud01_02.h"
#include "../art/cloud01_03.h"

// firstSpriteNum = index of first of 3 consecutive sprites used for the cloud
// memCharPos = index into character memory
// objRamPos = index into obj ram where first pixel belongs
void Cloud::Init(u8 firstSpriteNum, u16 memCharPos, u16 objRamPos, s16 xPos, s16 yPos)
{
	firstSpriteIndex = firstSpriteNum;
	xFirstSprite = xPos;
	yFirstSprite = yPos;
	fxGradualPos = xPos;
	fyGradualPos = yPos;
	firstMemCharPos = memCharPos;
	firstObjRamPos = objRamPos;

	sprites[firstSpriteNum].attribute0 = ATR0_COLOR_256 | ATR0_SHAPE_SQUARE | yPos;
	sprites[firstSpriteNum].attribute1 = ATR1_SIZE_32 | xPos;
	sprites[firstSpriteNum].attribute2 = memCharPos | ATR2_PRIORITY(1);

	sprites[firstSpriteNum+1].attribute0 = ATR0_COLOR_256 | ATR0_SHAPE_SQUARE | yPos;
	sprites[firstSpriteNum+1].attribute1 = ATR1_SIZE_32 | (xPos + 32);
	sprites[firstSpriteNum+1].attribute2 = (memCharPos + 32) | ATR2_PRIORITY(1);

	sprites[firstSpriteNum+2].attribute0 = ATR0_COLOR_256 | ATR0_SHAPE_SQUARE | yPos;
	sprites[firstSpriteNum+2].attribute1 = ATR1_SIZE_32 | (xPos + 64);
	sprites[firstSpriteNum+2].attribute2 = (memCharPos + 64) | ATR2_PRIORITY(1);

	u16* objMemStart = (u16*)OBJ_RAM_START;

	// copy sprite bitmap data
	u16 objRamLoop = objRamPos+512;
	int i;
	for(i=objRamPos; i<objRamLoop; i++)
		objMemStart[i] = cloud01_01Data[i-objRamPos];

	objRamLoop = objRamPos+1024;
	for(i=objRamPos+512; i<objRamLoop; i++)
		objMemStart[i] = cloud01_02Data[i-objRamPos-512];

	objRamLoop = objRamPos+1536;
	for(i=objRamPos+1024; i<objRamLoop; i++)
		objMemStart[i] = cloud01_03Data[i-objRamPos-1024];

	xVelocity = 0;
	yVelocity = 0;
}

void Cloud::SetPos(s16 xPos, s16 yPos)
{
	xFirstSprite = xPos;
	yFirstSprite = yPos;

	MoveSprite(&sprites[firstSpriteIndex],   xPos,    yPos);
	MoveSprite(&sprites[firstSpriteIndex+1], xPos+32, yPos);
	MoveSprite(&sprites[firstSpriteIndex+2], xPos+64, yPos);
}

void Cloud::SetVel(float xVel, float yVel)
{
	xVelocity = xVel;
	yVelocity = yVel;
}

// uses current vel and pos to calculate new pos.
// moves the cloud until it reaches edge of screen then starts over from left side after wrap around
void Cloud::Update()
{
	fxGradualPos += xVelocity;

	// wrap around
	if((int)fxGradualPos >= SCREEN_WIDTH)
	{
		SetPos(512-CLOUD_WIDTH, yFirstSprite);
		fxGradualPos = -CLOUD_WIDTH;
	}
	else
	{
		if(fxGradualPos < 0)
			SetPos((s16)(512+fxGradualPos), yFirstSprite);
		else
			SetPos((s16)(fxGradualPos), yFirstSprite);
	}
}

void Cloud::Delete()
{
	int i=0;
	int j=0;
	
	i = firstSpriteIndex + 3;
	for(j=firstSpriteIndex; j<i; j++)
	{
		// set offscreen
		sprites[j].attribute0 = 160;
		sprites[j].attribute1 = 240;
		sprites[j].attribute2 = 0;
	}

	// zero out the sprite mem
	u16* objMemStart = (u16*)OBJ_RAM_START;
	i = firstObjRamPos + 512;
	for(j=firstObjRamPos; j<i; j++)
		objMemStart[j] = (u16)0;
}
