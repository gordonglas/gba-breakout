#include "gba.h"
#include "sprite.h"

u16* g_ObjRam = (u16*)OBJ_RAM_START;	// object ram (sprite memory) ptr

OAMEntry sprites[128];
SpriteEx spritesEx[128];
pRotData rotData = (pRotData)sprites; // point to same mem
s16 nSpriteCreationIndex = 0;
u16 nCurrentMaxCharWidth = 0;
u16 nCurrentMaxCharNum = 0;

/* SpriteSetPalette - sets the sprite palette
Param
  pPaletteData - palette data
*/
void SpriteSetPalette(const u16* pPaletteData)
{
	u16* pPalMem = (u16*)OBJ_PALETTE_MEM;

	for(u16 i = 0; i < 256; i++)
		pPalMem[i] = pPaletteData[i];
}

/* SpriteLoadImage - Loads image data into gba's character memory,
                     returns start location of data in character memory
Params
  pImageData - pointer to image data to load
  nWidth     - width of entire image in pixels
  nHeight    - height of entire image in pixels
  nColors    - max number of colors image could contain (can be 16 or 256)
Return
  >= 0       - character memory location where image resides: pass this value to SpriteCreate
*/
u16 SpriteLoadImage(const u16* pImageData, u16 nWidth, u16 nHeight, u16 nColors)
{
	u16 nStartCharNum = nCurrentMaxCharNum;

	// formula for calculating character memory width
	// in order to correctly copy the image into memory:
	/*
	ret = IMAGE_WIDTH * 8;
	if(IMAGE_COLOR == 256)
		ret = ret / 2;
	ret = ret * (IMAGE_HEIGHT / 8);
	*/

	u16 nCharWidth = nWidth << 3;
	if(nColors == 256)
		nCharWidth >>= 1;
	nCharWidth *= (nHeight >> 3);

	u16 nHighIndex = nCurrentMaxCharWidth + nCharWidth;

	for(u16 i = nCurrentMaxCharWidth; i < nHighIndex; i++)
		g_ObjRam[i] = pImageData[i - nCurrentMaxCharWidth];

	nCurrentMaxCharWidth += nCharWidth;

	// formula for calculating the index into character memory
	// that coresponds to this image. You will pass it to SpriteCreate
	/*
	ret = (IMAGE_WIDTH / 8) * (IMAGE_HEIGHT / 8)
	if(IMAGE_COLOR == 256)
		ret = ret * 2;
	*/

	u16 nCharNum = (nWidth >> 3) * (nHeight >> 3);
	if(nColors == 256)
		nCharNum <<= 1;

	nCurrentMaxCharNum += nCharNum;

	return nStartCharNum;
}

/* SpriteCreate - creates a sprite in the sprites[] array, returns the sprite's index in the array
TODO:
  Add support for:
    scaling/rotation
    alpha blending
    mosaics
    horizontal/vertical flip bits (might be better to use another function for this)

Params
  Required:
    nCharMemPos - position in character memory where image data is stored. This is returned from SpriteLoadImage().
    nPosX       - initial x position of sprite
    nPosY       - initial y position of sprite
    ffixVelX    - initial x velocity of sprite (float fixed format)
    ffixVelY    - initial y velocity of sprite (float fixed format)
    nPriority   - the priority (valid values are 0 through 3, 0 is top-most)
    nSize       - size of sprite (use SPRITE_SIZE_ #defines)
    nShape      - shape of sprite (use SPRITE_SHAPE_ #defines)
    nColors     - number of colors in the sprite (use SPRITE_COLOR_ #defines)
  Optional:
    nPalIndex   - if nColor = SPRITE_COLOR_16, this is the palette index the sprite will use

Return
  -1         - error
  >= 0       - index of newly created sprite
*/
s16 SpriteCreate(u16 nCharMemPos, s16 nPosX, s16 nPosY, FIXED ffixVelX, FIXED ffixVelY,
				 u8 nPriority, u16 nSize, u16 nShape, u16 nColors, u16 nPalIndex)
{
	s16 nIndex = FindOpenSpriteIndex();
	if(nIndex == -1)
		return -1;

	sprites[nIndex].attribute0 = nColors | nShape | nPosY;
	sprites[nIndex].attribute1 = nSize | nPosX;
	sprites[nIndex].attribute2 = nCharMemPos | ATR2_PRIORITY(nPriority) | ATR2_PALETTE(nPalIndex);

	spritesEx[nIndex].ffixVelX = ffixVelX;
	spritesEx[nIndex].ffixVelY = ffixVelY;

	// mark sprite as being used
	spritesEx[nIndex].nState = SPRITE_STATE_ALIVE;

	nSpriteCreationIndex++;

	return nIndex;
}

/* SpriteCopy - copies a sprite
Params
  nSpriteIndex - index of sprite to be copied
Return
  -1           - error
  >= 0         - index of newly created sprite
*/
s16 SpriteCopy(u8 nSpriteIndex)
{
	s16 nIndex = FindOpenSpriteIndex();
	if(nIndex == -1)
		return -1;

	sprites[nIndex].attribute0 = sprites[nSpriteIndex].attribute0;
	sprites[nIndex].attribute1 = sprites[nSpriteIndex].attribute1;
	sprites[nIndex].attribute2 = sprites[nSpriteIndex].attribute2;

	spritesEx[nIndex].ffixVelX = spritesEx[nSpriteIndex].ffixVelX;
	spritesEx[nIndex].ffixVelY = spritesEx[nSpriteIndex].ffixVelY;

	spritesEx[nIndex].nState = spritesEx[nSpriteIndex].nState;

	nSpriteCreationIndex++;

	return nIndex;
}

// deletes all sprites
void DeleteAllSprites(void)
{
	DeleteSpriteRange(0, 128);
}

// deletes one sprite
void DeleteSprite(u8 nIndex)
{
	sprites[nIndex].attribute0 = 160;	//y to > 159
	sprites[nIndex].attribute1 = 240;	//x to > 239
	sprites[nIndex].attribute2 = 0;
	spritesEx[nIndex].nState = SPRITE_STATE_NULL;

	nSpriteCreationIndex = nIndex;
}

// deletes sprites from nFirstIndex up to (but not including) nLastIndex
void DeleteSpriteRange(u8 nFirstIndex, u8 nLastIndex)
{
	for(u8 i = nFirstIndex; i < nLastIndex; i++)
	{
		// set sprite offscreen
		// and mark as null (unused)
		sprites[i].attribute0 = 160;	//y to > 159
		sprites[i].attribute1 = 240;	//x to > 239
		sprites[i].attribute2 = 0;
		spritesEx[i].nState = SPRITE_STATE_NULL;
	}

	nSpriteCreationIndex = nFirstIndex;
}

/* FindOpenSpriteIndex - finds an unused sprite index in the sprite array
Return
  -1    - error: no open index found!
  >= 0  - unused sprite index
*/
s16 FindOpenSpriteIndex(void)
{
	if(nSpriteCreationIndex == -1)
		return -1;

	if(nSpriteCreationIndex == 128)
		nSpriteCreationIndex = 0;

	do
	{
		// if state is null, it is unused
		if(spritesEx[nSpriteCreationIndex].nState == SPRITE_STATE_NULL)
			return nSpriteCreationIndex;

		nSpriteCreationIndex++;
	}
	while (nSpriteCreationIndex < 128);

	nSpriteCreationIndex = -1;
	return -1;
}

void InitSpriteMemory(void)
{
	nCurrentMaxCharWidth = 0;
	nCurrentMaxCharNum = 0;
	//DeleteAllSprites();
	//nSpriteCreationIndex = 0;
}

/*
void CopyOAM(void)
{
	// render all sprites using DMA
	REG_DMA3SAD = (u32)sprites;
	REG_DMA3DAD = (u32)OAM_MEM_START;
	BARRIER();
	REG_DMA3CNT = 512 | DMA_16NOW;	// 128 sprites * 4 u16's = 512
}
*/

/* SpriteSetPos - sets x and y of a sprite
Params
  nSpriteIndex - index of sprite whose position we want to set
  nPosX        - x position
  nPosY        - y position
*/
void SpriteSetPos(u8 nSpriteIndex, s16 nPosX, s16 nPosY)
{
	//clear the old x value (first 9 bits of attribute 1)
	sprites[nSpriteIndex].attribute1 = sprites[nSpriteIndex].attribute1 & 0xFE00;
	sprites[nSpriteIndex].attribute1 = sprites[nSpriteIndex].attribute1 | nPosX;

	//clear the old y value (first 8 bits of attribute 0)
	sprites[nSpriteIndex].attribute0 = sprites[nSpriteIndex].attribute0 & 0xFF00;
	sprites[nSpriteIndex].attribute0 = sprites[nSpriteIndex].attribute0 | nPosY;
}

/*
// this is only here for compatilbility and will be deleted soon
//   -gordon
void MoveSprite(OAMEntry* sp, int x, int y)
{
	//clear the old x value (first 9 bits of attribute 1)
	sp->attribute1 = sp->attribute1 & 0xFE00;
	sp->attribute1 = sp->attribute1 | x;

	//clear the old y value (first 8 bits of attribute 0)
	sp->attribute0 = sp->attribute0 & 0xFF00;
	sp->attribute0 = sp->attribute0 | y;
}
*/

void SpriteSetAlphaMode(u8 nSpriteIndex, u16 nAlphaMode)
{
	//clear the old alpha mode (bits 11 and 12)
	sprites[nSpriteIndex].attribute0 = sprites[nSpriteIndex].attribute0 & 0xF3FF;
	sprites[nSpriteIndex].attribute0 = sprites[nSpriteIndex].attribute0 | nAlphaMode;
}
