#define INTERUPT_C

#include "gba.h"
#include "sprite.h"
#include "interupts.h"
#include "../../music/GbaPlayer.h"

// interupt function defs
void VBLANK(void) IN_IWRAM;
void HBLANK(void);  //put this in ram to make it fast
                    //you may also notice that I make this with arm instructions
                    //just look in the make.bat
void DMA0(void);
void DMA1(void) IN_IWRAM;
void DMA2(void);
void DMA3(void);
void VCOUNT(void);
void TIMMER0(void);
void TIMMER1(void);
void TIMMER2(void);
void TIMMER3(void);
void KEYBOARD(void);
void CART(void);
void COMUNICATION(void);

/*
// bg offsets and velocities
FIXED fxhofs = 0;
FIXED fxvofs = 0;
FIXED fxhvel = FLOAT_TO_FIXED(0.2);
FIXED fxvvel = FLOAT_TO_FIXED(0.2);
*/

/*---------------------------------------some externals------------------------*/
/*
extern FIXED SIN[360];      //my LUTs for sign and cosign
extern FIXED COS[360];
extern FIXED div[320];
extern int angle;
extern FIXED x;      //x and y background scroll
extern FIXED y;
extern int height;     //height of the player
*/

/*-------enable/disable interupt rutine---------------*/
//here is a function to enable interupts.  If it is VBLANK or HBLANK it sets REG_DISPSTAT to the apropriate values
//all others you will have to do on your own.
void EnableInterupts(u16 interupts)
{
    REG_IME = 0;  //probably not necessary but not a good idea to change interupt registers when an interupt is ocuring

    
    if(interupts | INT_VBLANK)
        REG_DISPSTAT |= 0x8;

    if(interupts | INT_HBLANK)
        REG_DISPSTAT |= 0x10;
    

    REG_IE |= interupts;

    REG_IME = 1;
}

void DisableInterupts(u16 interupts)
{
    REG_IE &= ~interupts;

    if ((interupts = INT_ALL)) //this is were that ALL comes in
        REG_IME = 0;  //disable all interupts;
}

//here is our table of function pointers.  I added the definition of fp to gba.h because I figure it would be usefull
//to have elsewhere
//typedef void (*fp)(void);   this is the definition you will find in gba.h.  It is just delaring fp to represent a pointer
//to a function

//here is our table of functions.  Once interupts are enabled in crt0.s you will not be able to link your code if this table is not pressent.
//the interupts must be listed in that order.
fp IntrTable[]  = 
{
    VBLANK,
    HBLANK,
    VCOUNT,
    TIMMER0,
    TIMMER1,
    TIMMER2,
    TIMMER3,
    COMUNICATION,
    DMA0,
    DMA1,
    DMA2,
    DMA3,
    KEYBOARD,
    CART
};

void VBLANK()
{
	// Update song
	MP_UpdateArm();

	//fpDrawTiles();
	g_bDraw = 1;

	/*
	// update scrolling bg
	fxhofs += fxhvel;
	fxvofs += fxvvel;

	if(FIXED_INT_PART(fxhofs) > 0)
		REG_BG2HOFS = (u16)(512-FIXED_INT_PART(fxhofs));  // this actually scrolls to the left (move image over to the right)
	else
		REG_BG2HOFS = (u16)FIXED_INT_PART(fxhofs);		// while this scrolls to the right (moves image to the left)

	if(FIXED_INT_PART(fxvofs) > 0)
		REG_BG2VOFS = (u16)(255-FIXED_INT_PART(fxvofs));	// this actually scrolls up (moves image down)
	else
		REG_BG2VOFS = (u16)FIXED_INT_PART(fxvofs);		// while this scrolls down (moves image up)
	*/

	//CopyOAM();

	// render all sprites using DMA
	//u16* OAM = (u16*)0x7000000;
	REG_DMA3SAD = (u32)sprites;
	REG_DMA3DAD = (u32)OAM_MEM_START;
	BARRIER();
	REG_DMA3CNT = 512 | DMA_16NOW;	// 128 sprites * 4 u16's = 512

    REG_IF |= INT_VBLANK;
}
void HBLANK(void) 
{
    REG_IF |= INT_HBLANK;
}
void DMA0(void)
{
    REG_IF |= INT_DMA0;
}
void DMA1(void)
{
    MP_IntHandler();			// call to mod player's interrupt handler
    REG_IF |= INT_DMA1;
}
void DMA2(void)
{
    REG_IF |= INT_DMA2;
}
void DMA3(void)
{
    REG_IF |= INT_DMA3;
}
void VCOUNT(void)
{
    REG_IF |= INT_VCOUNT;
}
void TIMMER0(void)
{
    REG_IF |= INT_TIMMER0;
}
void TIMMER1(void)
{
    REG_IF |= INT_TIMMER1;
}
void TIMMER2(void)
{
    REG_IF |= INT_TIMMER2;
}
void TIMMER3(void)
{
    REG_IF |= INT_TIMMER3;
}
void KEYBOARD(void)
{
    REG_IF |= INT_TIMMER3;
}
void CART(void)
{
    REG_IF |= INT_CART;
}
void COMUNICATION(void)
{
    REG_IF |= INT_COMUNICATION;
}
