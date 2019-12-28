///////////////////////////////////////////////////////////////////////////////
// gba.h
///////////////////////////////////////////////////////////////////////////////

#ifndef GBA_H
#define GBA_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef unsigned char byte;
typedef unsigned short hword;
typedef unsigned int word;

// fixed point macros

typedef s32 FIXED;
/*
#ifdef FIXPOINT16_16

#define INT_TO_FIXED(n)		(FIXED(((n) << 16)))
#define FLOAT_TO_FIXED(n)	(FIXED((float)(n) * 65536))
#define FIXED_INT_PART(n)	((n) >> 16)
#define FIXED_DEC_PART(n)	((n) & 0x0000ffff)
#define FIXED_MULTIPLY(n,m) (FIXED(((n) * (m)) >> 16))
#define FIXED_DIVIDE(n,m)	(FIXED(((n) << 16) / (m)))

#endif
*/
//#ifdef FIXPOINT24_8

#define INT_TO_FIXED(n)		(FIXED(((n) << 8)))
#define FLOAT_TO_FIXED(n)	(FIXED((float)(n) * 256))
#define FIXED_INT_PART(n)	((n) >> 8)
#define FIXED_DEC_PART(n)	((n) & 0x000000ff)
#define FIXED_MULTIPLY(n,m) (FIXED(((n) * (m)) >> 8))
#define FIXED_DIVIDE(n,m)	(FIXED(((n) << 8) / (m)))

//#endif

typedef void (*fp)(void);

extern u8 g_bDraw;

#define NULL 0

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define ABS(a)   (((a) > 0)   ? (a) : (-a))

#define FALSE                0x00
#define TRUE                 0x01

// screen defines
#define SCREEN_WIDTH         240
#define SCREEN_HEIGHT        160

#define IN_EWRAM __attribute__ ((section (".ewram")))
#define IN_IWRAM __attribute__ ((section (".iwram")))

// memory register defines
#define OAM_MEM_START        0x7000000
#define BG_RAM_START         0x6000000
#define OBJ_RAM_START        0x6010000
#define BG_PALETTE_MEM       0x5000000
#define OBJ_PALETTE_MEM      0x5000200

// buffer defines
#define BACK_BUFFER          (u16*)0x600A000

// DMA defines
#define BARRIER() asm volatile ("")

#define REG_DMA0SAD				*(u32*)0x40000B0
#define REG_DMA0DAD				*(u32*)0x40000B4
#define REG_DMA0CNT				*(u32*)0x40000B8
#define REG_DMA1SAD				*(u32*)0x40000BC
#define REG_DMA1DAD				*(u32*)0x40000C0
#define REG_DMA1CNT				*(u32*)0x40000C4
#define REG_DMA2SAD				*(u32*)0x40000C8
#define REG_DMA2DAD				*(u32*)0x40000CC
#define REG_DMA2CNT				*(u32*)0x40000D0
#define REG_DMA3SAD				*(u32*)0x40000D4
#define REG_DMA3DAD				*(u32*)0x40000D8
#define REG_DMA3CNT				*(u32*)0x40000DC
#define DMA_16					0x00000000
#define DMA_32					0x04000000
#define HALF_WORD_DMA			0x00000000
#define DMA_TIMEING_IMMEDIATE	0x00000000
#define DMA_ENABLE				0x80000000
#define DMA_INTERUPT_ENABLE     0x40000000
#define DMA_START_ON_FIFO_EMPTY         0x30000000
#define DMA_TIMEING_SYNC_TO_DISPLAY     0x30000000
#define START_ON_NEXT_VBLANK	0x10000000
#define DMA_TIMEING_HBLANK      0x20000000
#define DMA_REPEAT				0x02000000
#define DEST_REG_SAME			0x00400000
#define DMA_CHANNEL_0			0x0
#define DMA_CHANNEL_1			0x1
#define DMA_CHANNEL_2			0x2
#define DMA_CHANNEL_3			0x3
#define DMA_CHANNEL_DEFAULT		0x3
#define DMA_SOURCE_INCREMENT            0x00000000
#define DMA_SOURCE_DECREMENT            0x00800000
#define DMA_SOURCE_FIXED                0x01000000
#define DMA_DEST_INCREMENT              0x00000000
#define DMA_DEST_DECREMENT              0x00200000
#define DMA_DEST_FIXED                  0x00400000
#define DMA_DEST_RELOAD                 0x00600000

#define DMA_32NOW      DMA_ENABLE | DMA_TIMEING_IMMEDIATE |DMA_32 
#define DMA_16NOW      DMA_ENABLE | DMA_TIMEING_IMMEDIATE |DMA_16 

// background defines
#define REG_BG0CNT     *(u16*)0x4000008
#define REG_BG1CNT     *(u16*)0x400000A
#define REG_BG2CNT     *(u16*)0x400000C
#define REG_BG3CNT     *(u16*)0x400000E
#define REG_BG0HOFS    *(u16*)0x4000010
#define REG_BG0VOFS    *(u16*)0x4000012
#define REG_BG1HOFS    *(u16*)0x4000014
#define REG_BG1VOFS    *(u16*)0x4000016
#define REG_BG2HOFS    *(u16*)0x4000018
#define REG_BG2VOFS    *(u16*)0x400001A
#define REG_BG3HOFS    *(u16*)0x400001C
#define REG_BG3VOFS    *(u16*)0x400001E
#define REG_BG2PA      *(u16*)0x4000020
#define REG_BG2PB      *(u16*)0x4000022
#define REG_BG2PC      *(u16*)0x4000024
#define REG_BG2PD      *(u16*)0x4000026
#define REG_BG2X       *(u32*)0x4000028
#define REG_BG2Y       *(u32*)0x400002C
#define REG_BG3PA      *(u16*)0x4000030
#define REG_BG3PB      *(u16*)0x4000032
#define REG_BG3PC      *(u16*)0x4000034
#define REG_BG3PD      *(u16*)0x4000036
#define REG_BG3X       *(u32*)0x4000038
#define REG_BG3Y       *(u32*)0x400003C

// REG_BGxCNT defines
#define PRIORITY0		0x0
#define PRIORITY1		0x1
#define PRIORITY2		0x2
#define PRIORITY3		0x3
#define ENABLE_MOSAIC	0x40
#define COLOR_16		0x0
#define COLOR_256		0x80
#define ENABLE_ROT_WRAP	0x2000
#define TXTBG_256_256	0x0
#define TXTBG_512_256	0x4000
#define TXTBG_256_512	0x8000
#define TXTBG_512_512	0xC000
#define ROTBG_128_128	0x0
#define ROTBG_256_256	0x4000
#define ROTBG_512_512	0x8000
#define ROTBG_1024_1024	0xC000

#define CharBaseBlock(n)	(((n)*0x4000)+0x6000000)
#define ScreenBaseBlock(n)	(((n)*0x800)+0x6000000)
#define CHR_BLOCK  2
#define SCR_BLOCK  8


// input defines
#define REG_KEYS       *(volatile u16*)0x4000130

// mosaic defines
#define REG_MOSAIC     *(u32*)0x400004C
#define REG_MOSAIC_L   *(u32*)0x400004C
#define REG_MOSAIC_H   *(u32*)0x400004E

// display mode defines
#define REG_DISPCNT    *(u32*)0x4000000
#define REG_DISPCNT_L  *(u16*)0x4000000
#define REG_DISPCNT_H  *(u16*)0x4000002
#define REG_DISPSTAT   *(u16*)0x4000004

// sound hardware defines
#define REG_SOUND1CNT_L *(u16*)0x04000060       // sound 1 sweep control  
#define REG_SOUND1CNT_H *(u16*)0x04000062       // sound 1 length, wave duty and envelope control 
#define REG_SOUND1CNT_X *(u16*)0x04000064       // sound 1 frequency, reset and loop control 
#define REG_SOUND2CNT_L *(u16*)0x04000068       // sound 2 length, wave duty and envelope control 
#define REG_SOUND2CNT_H *(u16*)0x0400006C       // sound 2 frequency, reset and loop control 
#define REG_SOUND3CNT_L *(u16*)0x04000070       // sound 3 enable and wave ram bank control 
#define REG_SOUND3CNT_H *(u16*)0x04000072       // sound 3 sound length and output level control 
#define REG_SOUND3CNT_X *(u16*)0x04000074       // sound 3 frequency, reset and loop control 
#define REG_SOUND4CNT_L *(u16*)0x04000078       // sound 4 length, output level and envelope control 
#define REG_SOUND4CNT_H *(u16*)0x0400007C       // sound 4 noise parameters, reset and loop control 
#define REG_SOUNDCNT_L  *(u16*)0x04000080       // sound 1-4 output level and stereo control 
#define REG_SOUNDCNT_H  *(u16*)0x04000082       // direct sound control and sound 1-4 output ratio 
#define REG_SOUNDCNT_X  *(u32*)0x04000084       // master sound enable and sound 1-4 play status 
#define REG_SOUNDBIAS   *(u32*)0x04000088       // sound bias and amplitude resolution control 
#define REG_WAVE_RAM0_L *(u16*)0x04000090       // sound 3 samples 0-3 
#define REG_WAVE_RAM0_H *(u16*)0x04000092       // sound 3 samples 4-7 
#define REG_WAVE_RAM1_L *(u16*)0x04000094       // sound 3 samples 8-11 
#define REG_WAVE_RAM1_H *(u16*)0x04000096       // sound 3 samples 12-15 
#define REG_WAVE_RAM2_L *(u16*)0x04000098       // sound 3 samples 16-19 
#define REG_WAVE_RAM2_H *(u16*)0x0400009A       // sound 3 samples 20-23 
#define REG_WAVE_RAM3_L *(u16*)0x0400009C       // sound 3 samples 23-27 
#define REG_WAVE_RAM3_H *(u16*)0x0400009E       // sound 3 samples 28-31 
#define REG_FIFO_A      0x040000A0              // direct sound channel A samples 0-3 
#define REG_FIFO_A_L    0x040000A0              // direct sound channel A samples 0-1 
#define REG_FIFO_A_H    0x040000A2              // direct sound channel A samples 2-3 
#define REG_FIFO_B      0x040000A4              // direct sound channel B samples 0-3 
#define REG_FIFO_B_L    0x040000A4              // direct sound channel B samples 0-1 
#define REG_FIFO_B_H    0x040000A6              // direct sound channel B samples 2-3 

// timer defines
#define REG_TM0D        *(volatile u16*)0x4000100
#define REG_TM0CNT      *(u16*)0x4000102
#define REG_TM1D        *(volatile u16*)0x4000104
#define REG_TM1CNT      *(u16*)0x4000106
#define REG_TM2D        *(volatile u16*)0x4000108
#define REG_TM2CNT      *(u16*)0x400010A
#define REG_TM3D        *(volatile u16*)0x400010C
#define REG_TM3CNT      *(u16*)0x400010E
#define TIMER_ENABLED   0x0080
#define TIME_FREQUENCY_SYSTEM	0x0
#define TIME_FREQUENCY_64		0x1
#define TIME_FREQUENCY_256		0x2 // will overflow every second
#define TIME_FREQUENCY_1024		0x3
#define TIME_OVERFLOW			0x4
#define TIME_IRQ_ENABLE			0x40

// other defines
#define REG_VCOUNT     *(volatile u16*)0x4000006
#define REG_WIN0H      *(u16*)0x4000040
#define REG_WIN1H      *(u16*)0x4000042
#define REG_WIN0V      *(u16*)0x4000044
#define REG_WIN1V      *(u16*)0x4000046
#define REG_WININ      *(u16*)0x4000048
#define REG_WINOUT     *(u16*)0x400004A
#define REG_BLDMOD     *(u16*)0x4000050
#define REG_COLEV      *(u16*)0x4000052
#define REG_COLEY      *(u16*)0x4000054
#define REG_SGFIF0A    *(u32*)0x40000A0
#define REG_SCD0       *(u16*)0x4000120
#define REG_SCD1       *(u16*)0x4000122
#define REG_SCD2       *(u16*)0x4000124
#define REG_SCD3       *(u16*)0x4000126
#define REG_SCCNT      *(u32*)0x4000128
#define REG_SCCNT_L    *(u16*)0x4000128
#define REG_SCCNT_H    *(u16*)0x400012A
#define REG_P1         *(u16*)0x4000130
#define REG_P1CNT      *(u16*)0x4000132
#define REG_R          *(u16*)0x4000134
#define REG_HS_CTRL    *(u16*)0x4000140
#define REG_JOYRE      *(u32*)0x4000150
#define REG_JOYRE_L    *(u16*)0x4000150
#define REG_JOYRE_H    *(u16*)0x4000152
#define REG_JOYTR      *(u32*)0x4000154
#define REG_JOYTR_L    *(u16*)0x4000154
#define REG_JOYTR_H    *(u16*)0x4000156
#define REG_JSTAT      *(u32*)0x4000158
#define REG_JSTAT_L    *(u16*)0x4000158
#define REG_JSTAT_H    *(u16*)0x400015A
#define REG_IE         *(volatile u16*)0x4000200
#define REG_IF         *(volatile u16*)0x4000202
#define REG_PAUSE      *(u16*)0x4000300

// instruction prefetch enable register define
#define REG_WSCNT      *(u16*)0x4000204

// interrupt register defines
#define REG_INTR_HANDLER *(u32*)0x3007FFC
#define REG_IME          *(volatile u16*)0x4000208

/*
// interrupt flags for REG_IE
#define INT_VBLANK			0x0001
#define INT_HBLANK			0x0002 
#define INT_VCOUNT			0x0004
#define INT_TIMMER0			0x0008
#define INT_TIMMER1			0x0010
#define INT_TIMMER2			0x0020 
#define INT_TIMMER3			0x0040
#define INT_COMUNICATION	0x0080
#define INT_DMA0			0x0100
#define INT_DMA1			0x0200
#define INT_DMA2			0x0400
#define INT_DMA3			0x0800
#define INT_KEYBOARD		0x1000
#define INT_CART			0x2000
#define INT_ALL             0x4000

// interrupt handler enumeration
enum
{
    VBLANK_HANDLER,
    HBLANK_HANDLER,
    VCOUNT_HANDLER,
    TIMER0_HANDLER,
    TIMER1_HANDLER,
    TIMER2_HANDLER,
    TIMER3_HANDLER,
    SERIAL_COM_HANDLER,
    DMA0_HANDLER,
    DMA1_HANDLER,
    DMA2_HANDLER,
    DMA3_HANDLER,
    KEYS_HANDLER,
    CART_HANDLER,
    MAX_INTERRUPT_HANDLERS,
};
*/
// display (REG_DISPCNT) defines
#define BG0_ENABLE		 0x100
#define BG1_ENABLE		 0x200
#define BG2_ENABLE		 0x400
#define BG3_ENABLE		 0x800
#define OBJ_ENABLE		 0x1000
#define WIN0_ENABLE		 0x2000
#define WIN1_ENABLE		 0x4000
#define WINOBJ_ENABLE	 0x8000
#define BACK_BUFFER_FLAG 0x10
#define H_BLANK_OAM      0x20
#define OBJ_MAP_2D       0x0
#define OBJ_MAP_1D       0x40
#define FORCE_BLANK      0x80

// video modes
#define MODE0			0x0
#define MODE1			0x1
#define MODE2			0x2
#define MODE3			0x3
#define MODE4			0x4
#define MODE5			0x5

// input defines
#define KEY_MASK   0x3FF
#define KEY_A      0x001
#define KEY_B      0x002
#define KEY_SELECT 0x004
#define KEY_START  0x008
#define KEY_RIGHT  0x010
#define KEY_LEFT   0x020
#define KEY_UP     0x040
#define KEY_DOWN   0x080
#define KEY_R      0x100
#define KEY_L      0x200

// some structs
typedef void (*fp)(void);

// fx processor (REG_BLDMOD) defines
#define TARGET1_BG0			0x1
#define TARGET1_BG1			0x2
#define TARGET1_BG2			0x4
#define TARGET1_OBJPLAIN	0x10
#define TARGET1_BACKPLAIN	0x20
#define TARGET2_BG0			0x100
#define TARGET2_BG1			0x200
#define TARGET2_BG2			0x400
#define TARGET2_OBJPLAIN	0x1000
#define TARGET2_BACKPLAIN	0x2000
#define MODE_NOALPHA		0x0
#define MODE_NORMAL			0x40
#define MODE_FADEIN			0x80
#define MODE_FADEOUT		0xC0

// helper macros
#define GBA_RGB_15(r, g, b)        ((r) + (g<<5) + (b<<10))
#define SCANLINE_COUNTER           (*(volatile u16*)0x4000006)
#define INPUT                      (KEY_MASK & (~REG_KEYS))

#endif
