#ifndef __SPRITE_H__
#define __SPRITE_H__

// atribute 0
#define ATR0_ROTATION				0x100
#define ATR0_SIZE_DOUBLE			0x200
#define ATR0_ALPHA_MODE_NORMAL		0x0
#define ATR0_ALPHA_MODE_TRANSPARANT	0x400
#define ATR0_ALPHA_MODE_WINDOWED	0x800
#define ATR0_MOSAIC					0x1000
#define ATR0_COLOR_16				0x0000
#define ATR0_COLOR_256				0x2000
#define ATR0_SHAPE_SQUARE			0x0
#define ATR0_SHAPE_TALL				0x8000
#define ATR0_SHAPE_WIDE				0x4000

// atribute 1
#define ATR1_ROTDATA(n)				((n) << 9)
#define ATR1_HORIZONTAL_FLIP		0x1000
#define ATR1_VERTICAL_FLIP			0x2000
#define ATR1_SIZE_8					0x0
#define ATR1_SIZE_16				0x4000
#define ATR1_SIZE_32				0x8000
#define ATR1_SIZE_64				0xC000

// atribute 2
#define ATR2_PRIORITY(n)			((n) << 10)
#define ATR2_PALETTE(n)				((n) << 12)

// for SpriteLoadImage()
#define IMAGE_COLOR_16				0x0
#define IMAGE_COLOR_256				0x2000

// for SpriteCreate()
#define SPRITE_SIZE_8				0x0
#define SPRITE_SIZE_16				0x4000
#define SPRITE_SIZE_32				0x8000
#define SPRITE_SIZE_64				0xC000

#define SPRITE_SHAPE_SQUARE			0x0
#define SPRITE_SHAPE_WIDE			0x4000
#define SPRITE_SHAPE_TALL			0x8000

#define SPRITE_COLOR_16				0x0
#define SPRITE_COLOR_256			0x2000

// sprite states
#define SPRITE_STATE_NULL			0x0
#define SPRITE_STATE_ALIVE			0x1

// sprite info object
typedef struct tagOAMEntry
{
        u16 attribute0;
        u16 attribute1;
        u16 attribute2;
        u16 attribute3;	// unused
} OAMEntry, *pOAMEntry;

// sprite rot/scaling object
typedef struct tagRotData
{
        u16 filler1[3];	// unused
        u16 pa;
        u16 filler2[3];	// unused
        u16 pb; 
        u16 filler3[3];	// unused
        u16 pc; 
        u16 filler4[3];	// unused
        u16 pd;
} RotData, *pRotData;

// our own attributes
typedef struct tagSpriteEx
{
	u8    nState;
	FIXED ffixVelX;
	FIXED ffixVelY;
} SpriteEx, *pSpriteEx;

extern u16* g_ObjRam;
extern OAMEntry sprites[128];
extern SpriteEx spritesEx[128];
extern pRotData rotData;

// "public" functions
void SpriteSetPalette(const u16* pPaletteData);
u16 SpriteLoadImage(const u16* pImageData, u16 nWidth, u16 nHeight, u16 nColors);
s16 SpriteCreate(u16 nCharMemPos, s16 nPosX, s16 nPosY, FIXED ffixVelX, FIXED ffixVelY,
				 u8 nPriority, u16 nSize, u16 nShape, u16 nColors, u16 nPalIndex = 0);
s16 SpriteCopy(u8 nSpriteIndex);
void DeleteAllSprites(void);
void DeleteSprite(u8 nIndex);
void DeleteSpriteRange(u8 nFirstIndex, u8 nLastIndex);
void SpriteSetPos(u8 nSpriteIndex, s16 nPosX, s16 nPosY);
void SpriteSetAlphaMode(u8 nSpriteIndex, u16 nAlphaMode);
void InitSpriteMemory(void);

// "private" functions
s16 FindOpenSpriteIndex(void);

//__attribute__(( long_call )) void CopyOAM(void); // Copy our sprite array to OAM
//void MoveSprite(OAMEntry* sp, int x, int y);

//u16* OAM = (u16*)0x7000000;
//OAMEntry sprites[128];
//pRotData rotData = (pRotData)sprites; // point to same mem

#endif
