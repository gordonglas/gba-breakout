#ifndef __FONT_H__
#define __FONT_H__

#include "gba.h"

void TextClear(u8 nScreenBaseBlock);
void TextToBG(u8 nScreenBaseBlock, u16 nTileX, u16 nTileY, char * pText);

#endif
