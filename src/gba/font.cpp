#include "gba.h"
#include "font.h"

void TextClear(u8 nScreenBaseBlock)
{
	u16* pMapMem = (u16*)ScreenBaseBlock(nScreenBaseBlock);

	/*
	// clear out map with first tile
	for(int i=0; i<1024; i++)
		pMapMem[i] = 0;
	*/

	// only clear out 18th line of screen (row of text at bottom of screen)
	// This is an optimization compared to the code that is commented out above.

	// (18 * 32)+1 = 577 (start)
	// (start + 30) = end

	for(u16 i = 577; i < 607; i++)
		pMapMem[i] = 0;
}

void TextToBG(u8 nScreenBaseBlock, u16 nTileX, u16 nTileY, char * pText)
{
	u16 i;
	u16 nMapTile;
	u8 nAscii;
	char nNewAscii = 0;
	u16* pMapMem = (u16*)ScreenBaseBlock(nScreenBaseBlock);

	nMapTile = (nTileY << 5) + nTileX;

	i = 0;
	while (pText[i] != '\0')
	{
		nAscii = pText[i];

		if(nAscii > 47 && nAscii < 58)			// numeric
			nNewAscii = nAscii-47;
		else if(nAscii > 64 && nAscii < 91)		// alpha
			nNewAscii = nAscii-54;
		else if(nAscii == 33)					// '!'
			nNewAscii = 37;
		else if(nAscii == 63)					// '?'
			nNewAscii = 38;
		else if(nAscii == 35)					// '#'
			nNewAscii = 39;
		else if(nAscii == 43)					// '+'
			nNewAscii = 40;
		else if(nAscii == 45)					// '-'
			nNewAscii = 41;
		else if(nAscii == 47)					// '/'
			nNewAscii = 42;
		else if(nAscii == 34)					// '"'
			nNewAscii = 43;
		else if(nAscii == 44)					// ','
			nNewAscii = 44;
		else if(nAscii == 46)					// '.'
			nNewAscii = 45;
		else if(nAscii == 39)					// '''
			nNewAscii = 46;
		else
			nNewAscii = 0;

		pMapMem[nMapTile] = nNewAscii;

		if(nMapTile < 1023)
			nMapTile++;
		else
			break;

		i++;
	}
}
