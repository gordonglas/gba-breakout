/*
	Background memory functions
	©2003 Illusive Studios
	www.illusivestudios.com
*/

#include "background.h"

/* BGSetCharBlock - binds a character base memory block to a background
Params
  nRegBGXCNT - address of background to change (use REG_BGXCNT defines)
  nCharBlock - character base block number (can be 0 through 3)
*/
void BGSetCharBlock(u16 &nRegBGXCNT, u8 nCharBlock)
{
	// clear char base block bits before setting them
	nRegBGXCNT &= 0xFFF3;
	nRegBGXCNT |= nCharBlock << CHR_BLOCK;
}

/* BGSetScreenBlock - binds a screen base memory block to a background
Params
  nRegBGXCNT   - address of background to change (use REG_BGXCNT defines)
  nScreenBlock - screen base block number (can be 0 through 31)
*/
void BGSetScreenBlock(u16 &nRegBGXCNT, u8 nScreenBlock)
{
	// clear screen base block bits before setting them
	nRegBGXCNT &= 0xE0FF;
	nRegBGXCNT |= nScreenBlock << SCR_BLOCK;
}

/* BGSetPriority - sets the priority of a background
Params
  nRegBGXCNT - address of background to change (use REG_BGXCNT defines)
  nPriority  - the priority (can be 0 through 3)
*/
void BGSetPriority(u16 &nRegBGXCNT, u8 nPriority)
{
	// clear priority bits before setting them
	nRegBGXCNT &= 0xFFFC;
	nRegBGXCNT |= nPriority;
}

/* BGSetPalette - sets the background palette
Param
  pPaletteData - palette data
*/
void BGSetPalette(const u16* pPaletteData)
{
	u16* pPalMem = (u16*)BG_PALETTE_MEM;
	for(u16 i = 0; i < 256; i++)
		pPalMem[i] = pPaletteData[i];
}

/* BGLoadImage - loads image tile data into a block of character memory
Params
  nCharBlock - character base block number (can be 0 through 3)
  pImageData - pointer to image data to be loaded
  nWidth     - width of image in pixels
  nHeight    - height of image in pixels
  nColors    - number of total posible colors in image (can be 16 or 256)
*/
void BGLoadImage(u8 nCharBlock, const u16* pImageData, u16 nWidth, u16 nHeight, u16 nColors)
{
	u16 nSize = nWidth * nHeight;
	if(nColors == 256)
		nSize >>= 1;

	u16* pBGMem = (u16*)CharBaseBlock(nCharBlock);
	for(u16 i = 0; i < nSize; i++)
		pBGMem[i] = pImageData[i];
}

/* BGLoadMap - loads map data into a screen base memory block
Params
  nScreenBlock - screen base block number (can be 0 through 31)
  pMapData     - pointer to map data to be loaded
*/
void BGLoadMap(u8 nScreenBlock, const u8* pMapData)
{
	u16* pMapMem = (u16*)ScreenBaseBlock(nScreenBlock);
	for (u16 i = 0; i < 1024; i++)
		pMapMem[i] = pMapData[i];
}

/* BGFillMap - fill a map with a single tile
Params
  nScreenBlock - screen base block number (can be 0 through 31)
  nTileIndex   - tile index that will be filled
*/
void BGFillMap(u8 nScreenBlock, u8 nTileIndex)
{
	u16* pMapMem = (u16*)ScreenBaseBlock(nScreenBlock);
	for(u16 i = 0; i < 1024; i++)
		pMapMem[i] = nTileIndex;
}
