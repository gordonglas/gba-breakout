// Background memory functions

#ifndef __BACKGROUND_MEM_H__
#define __BACKGROUND_MEM_H__

#include "gba.h"

// background setup functions
void BGSetCharBlock(u16 &nRegBGXCNT, u8 nCharBlock);
void BGSetScreenBlock(u16 &nRegBGXCNT, u8 nScreenBlock);
void BGSetPriority(u16 &nRegBGXCNT, u8 nPriority);

void BGSetPalette(const u16* pPaletteData);
void BGLoadImage(u8 nCharBlock, const u16* pImageData, u16 nWidth, u16 nHeight, u16 nColors);
void BGLoadMap(u8 nScreenBlock, const u8* pMapData);
void BGFillMap(u8 nScreenBaseBlock, u8 nTileIndex);

#endif // __BACKGROUND_MEM_H__
