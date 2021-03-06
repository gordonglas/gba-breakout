#include "util.h"

u8 random8[64] = { 215, 25, 169, 8, 231, 201, 91, 15, 156, 5, 152, 30, 159,  50, 22, 49,
              28, 148, 157, 123, 58, 3, 156, 236, 46, 78, 50, 157, 186, 170, 233, 138,
              49, 98, 215, 231, 0, 249, 233, 254, 194, 12, 110, 115, 122, 166, 15, 2, 163,
              237, 143, 30, 54, 5, 45, 199, 201, 106, 184, 136, 44, 175, 197, 11 };
u8 seed8_1=29, seed8_2=63;
u8 rand8()
{
    u8 val = random8[seed8_1];
    val += random8[seed8_2];
    random8[seed8_2] = val;
    seed8_1 += 1; seed8_1 &= 63;
    seed8_2 += 1; seed8_2 &= 63;
    return val;
}

// lookup tables for Sine and Cosine

// SIN table for 24.8 fixed format
FIXED SIN[360]=
{	0x00000000,
	0x00000004,	0x00000008,	0x0000000D,	0x00000011,	0x00000016,	0x0000001A,	0x0000001F,	0x00000023,	0x00000028,	0x0000002C,
	0x00000030,	0x00000035,	0x00000039,	0x0000003D,	0x00000042,	0x00000046,	0x0000004A,	0x0000004F,	0x00000053,	0x00000057,
	0x0000005B,	0x0000005F,	0x00000064,	0x00000068,	0x0000006C,	0x00000070,	0x00000074,	0x00000078,	0x0000007C,	0x00000080,
	0x00000083,	0x00000087,	0x0000008B,	0x0000008F,	0x00000092,	0x00000096,	0x0000009A,	0x0000009D,	0x000000A1,	0x000000A4,
	0x000000A7,	0x000000AB,	0x000000AE,	0x000000B1,	0x000000B5,	0x000000B8,	0x000000BB,	0x000000BE,	0x000000C1,	0x000000C4,
	0x000000C6,	0x000000C9,	0x000000CC,	0x000000CF,	0x000000D1,	0x000000D4,	0x000000D6,	0x000000D9,	0x000000DB,	0x000000DD,
	0x000000DF,	0x000000E2,	0x000000E4,	0x000000E6,	0x000000E8,	0x000000E9,	0x000000EB,	0x000000ED,	0x000000EE,	0x000000F0,
	0x000000F2,	0x000000F3,	0x000000F4,	0x000000F6,	0x000000F7,	0x000000F8,	0x000000F9,	0x000000FA,	0x000000FB,	0x000000FC,
	0x000000FC,	0x000000FD,	0x000000FE,	0x000000FE,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x00000100,
	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FE,	0x000000FE,	0x000000FD,	0x000000FC,	0x000000FC,
	0x000000FB,	0x000000FA,	0x000000F9,	0x000000F8,	0x000000F7,	0x000000F6,	0x000000F4,	0x000000F3,	0x000000F2,	0x000000F0,
	0x000000EE,	0x000000ED,	0x000000EB,	0x000000E9,	0x000000E8,	0x000000E6,	0x000000E4,	0x000000E2,	0x000000DF,	0x000000DD,
	0x000000DB,	0x000000D9,	0x000000D6,	0x000000D4,	0x000000D1,	0x000000CF,	0x000000CC,	0x000000C9,	0x000000C6,	0x000000C4,
	0x000000C1,	0x000000BE,	0x000000BB,	0x000000B8,	0x000000B5,	0x000000B1,	0x000000AE,	0x000000AB,	0x000000A7,	0x000000A4,
	0x000000A1,	0x0000009D,	0x0000009A,	0x00000096,	0x00000092,	0x0000008F,	0x0000008B,	0x00000087,	0x00000083,	0x0000007F,
	0x0000007C,	0x00000078,	0x00000074,	0x00000070,	0x0000006C,	0x00000068,	0x00000064,	0x0000005F,	0x0000005B,	0x00000057,
	0x00000053,	0x0000004F,	0x0000004A,	0x00000046,	0x00000042,	0x0000003D,	0x00000039,	0x00000035,	0x00000030,	0x0000002C,
	0x00000028,	0x00000023,	0x0000001F,	0x0000001A,	0x00000016,	0x00000011,	0x0000000D,	0x00000008,	0x00000004,	0x00000000,
	0xFFFFFFFC,	0xFFFFFFF8,	0xFFFFFFF3,	0xFFFFFFEF,	0xFFFFFFEA,	0xFFFFFFE6,	0xFFFFFFE1,	0xFFFFFFDD,	0xFFFFFFD8,	0xFFFFFFD4,
	0xFFFFFFD0,	0xFFFFFFCB,	0xFFFFFFC7,	0xFFFFFFC3,	0xFFFFFFBE,	0xFFFFFFBA,	0xFFFFFFB6,	0xFFFFFFB1,	0xFFFFFFAD,	0xFFFFFFA9,
	0xFFFFFFA5,	0xFFFFFFA1,	0xFFFFFF9C,	0xFFFFFF98,	0xFFFFFF94,	0xFFFFFF90,	0xFFFFFF8C,	0xFFFFFF88,	0xFFFFFF84,	0xFFFFFF80,
	0xFFFFFF7D,	0xFFFFFF79,	0xFFFFFF75,	0xFFFFFF71,	0xFFFFFF6E,	0xFFFFFF6A,	0xFFFFFF66,	0xFFFFFF63,	0xFFFFFF5F,	0xFFFFFF5C,
	0xFFFFFF59,	0xFFFFFF55,	0xFFFFFF52,	0xFFFFFF4F,	0xFFFFFF4B,	0xFFFFFF48,	0xFFFFFF45,	0xFFFFFF42,	0xFFFFFF3F,	0xFFFFFF3C,
	0xFFFFFF3A,	0xFFFFFF37,	0xFFFFFF34,	0xFFFFFF31,	0xFFFFFF2F,	0xFFFFFF2C,	0xFFFFFF2A,	0xFFFFFF27,	0xFFFFFF25,	0xFFFFFF23,
	0xFFFFFF21,	0xFFFFFF1E,	0xFFFFFF1C,	0xFFFFFF1A,	0xFFFFFF18,	0xFFFFFF17,	0xFFFFFF15,	0xFFFFFF13,	0xFFFFFF12,	0xFFFFFF10,
	0xFFFFFF0E,	0xFFFFFF0D,	0xFFFFFF0C,	0xFFFFFF0A,	0xFFFFFF09,	0xFFFFFF08,	0xFFFFFF07,	0xFFFFFF06,	0xFFFFFF05,	0xFFFFFF04,
	0xFFFFFF04,	0xFFFFFF03,	0xFFFFFF02,	0xFFFFFF02,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF00,
	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF02,	0xFFFFFF02,	0xFFFFFF03,	0xFFFFFF04,	0xFFFFFF04,
	0xFFFFFF05,	0xFFFFFF06,	0xFFFFFF07,	0xFFFFFF08,	0xFFFFFF09,	0xFFFFFF0A,	0xFFFFFF0C,	0xFFFFFF0D,	0xFFFFFF0E,	0xFFFFFF10,
	0xFFFFFF12,	0xFFFFFF13,	0xFFFFFF15,	0xFFFFFF17,	0xFFFFFF18,	0xFFFFFF1A,	0xFFFFFF1C,	0xFFFFFF1E,	0xFFFFFF21,	0xFFFFFF23,
	0xFFFFFF25,	0xFFFFFF27,	0xFFFFFF2A,	0xFFFFFF2C,	0xFFFFFF2F,	0xFFFFFF31,	0xFFFFFF34,	0xFFFFFF37,	0xFFFFFF3A,	0xFFFFFF3C,
	0xFFFFFF3F,	0xFFFFFF42,	0xFFFFFF45,	0xFFFFFF48,	0xFFFFFF4B,	0xFFFFFF4F,	0xFFFFFF52,	0xFFFFFF55,	0xFFFFFF59,	0xFFFFFF5C,
	0xFFFFFF5F,	0xFFFFFF63,	0xFFFFFF66,	0xFFFFFF6A,	0xFFFFFF6E,	0xFFFFFF71,	0xFFFFFF75,	0xFFFFFF79,	0xFFFFFF7D,	0xFFFFFF81,
	0xFFFFFF84,	0xFFFFFF88,	0xFFFFFF8C,	0xFFFFFF90,	0xFFFFFF94,	0xFFFFFF98,	0xFFFFFF9C,	0xFFFFFFA1,	0xFFFFFFA5,	0xFFFFFFA9,
	0xFFFFFFAD,	0xFFFFFFB1,	0xFFFFFFB6,	0xFFFFFFBA,	0xFFFFFFBE,	0xFFFFFFC3,	0xFFFFFFC7,	0xFFFFFFCB,	0xFFFFFFD0,	0xFFFFFFD4,
	0xFFFFFFD8,	0xFFFFFFDD,	0xFFFFFFE1,	0xFFFFFFE6,	0xFFFFFFEA,	0xFFFFFFEF,	0xFFFFFFF3,	0xFFFFFFF8,	0xFFFFFFFC,
};

// COS table for 24.8 fixed format
FIXED COS[360]=
{	0x00000100,
	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FE,	0x000000FE,	0x000000FD,	0x000000FC,	0x000000FC,
	0x000000FB,	0x000000FA,	0x000000F9,	0x000000F8,	0x000000F7,	0x000000F6,	0x000000F4,	0x000000F3,	0x000000F2,	0x000000F0,
	0x000000EE,	0x000000ED,	0x000000EB,	0x000000E9,	0x000000E8,	0x000000E6,	0x000000E4,	0x000000E2,	0x000000DF,	0x000000DD,
	0x000000DB,	0x000000D9,	0x000000D6,	0x000000D4,	0x000000D1,	0x000000CF,	0x000000CC,	0x000000C9,	0x000000C6,	0x000000C4,
	0x000000C1,	0x000000BE,	0x000000BB,	0x000000B8,	0x000000B5,	0x000000B1,	0x000000AE,	0x000000AB,	0x000000A7,	0x000000A4,
	0x000000A1,	0x0000009D,	0x0000009A,	0x00000096,	0x00000092,	0x0000008F,	0x0000008B,	0x00000087,	0x00000083,	0x0000007F,
	0x0000007C,	0x00000078,	0x00000074,	0x00000070,	0x0000006C,	0x00000068,	0x00000064,	0x0000005F,	0x0000005B,	0x00000057,
	0x00000053,	0x0000004F,	0x0000004A,	0x00000046,	0x00000042,	0x0000003D,	0x00000039,	0x00000035,	0x00000030,	0x0000002C,
	0x00000028,	0x00000023,	0x0000001F,	0x0000001A,	0x00000016,	0x00000011,	0x0000000D,	0x00000008,	0x00000004,	0x00000000,
	0xFFFFFFFC,	0xFFFFFFF8,	0xFFFFFFF3,	0xFFFFFFEF,	0xFFFFFFEA,	0xFFFFFFE6,	0xFFFFFFE1,	0xFFFFFFDD,	0xFFFFFFD8,	0xFFFFFFD4,
	0xFFFFFFD0,	0xFFFFFFCB,	0xFFFFFFC7,	0xFFFFFFC3,	0xFFFFFFBE,	0xFFFFFFBA,	0xFFFFFFB6,	0xFFFFFFB1,	0xFFFFFFAD,	0xFFFFFFA9,
	0xFFFFFFA5,	0xFFFFFFA1,	0xFFFFFF9C,	0xFFFFFF98,	0xFFFFFF94,	0xFFFFFF90,	0xFFFFFF8C,	0xFFFFFF88,	0xFFFFFF84,	0xFFFFFF80,
	0xFFFFFF7D,	0xFFFFFF79,	0xFFFFFF75,	0xFFFFFF71,	0xFFFFFF6E,	0xFFFFFF6A,	0xFFFFFF66,	0xFFFFFF63,	0xFFFFFF5F,	0xFFFFFF5C,
	0xFFFFFF59,	0xFFFFFF55,	0xFFFFFF52,	0xFFFFFF4F,	0xFFFFFF4B,	0xFFFFFF48,	0xFFFFFF45,	0xFFFFFF42,	0xFFFFFF3F,	0xFFFFFF3C,
	0xFFFFFF3A,	0xFFFFFF37,	0xFFFFFF34,	0xFFFFFF31,	0xFFFFFF2F,	0xFFFFFF2C,	0xFFFFFF2A,	0xFFFFFF27,	0xFFFFFF25,	0xFFFFFF23,
	0xFFFFFF21,	0xFFFFFF1E,	0xFFFFFF1C,	0xFFFFFF1A,	0xFFFFFF18,	0xFFFFFF17,	0xFFFFFF15,	0xFFFFFF13,	0xFFFFFF12,	0xFFFFFF10,
	0xFFFFFF0E,	0xFFFFFF0D,	0xFFFFFF0C,	0xFFFFFF0A,	0xFFFFFF09,	0xFFFFFF08,	0xFFFFFF07,	0xFFFFFF06,	0xFFFFFF05,	0xFFFFFF04,
	0xFFFFFF04,	0xFFFFFF03,	0xFFFFFF02,	0xFFFFFF02,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF00,
	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF01,	0xFFFFFF02,	0xFFFFFF02,	0xFFFFFF03,	0xFFFFFF04,	0xFFFFFF04,
	0xFFFFFF05,	0xFFFFFF06,	0xFFFFFF07,	0xFFFFFF08,	0xFFFFFF09,	0xFFFFFF0A,	0xFFFFFF0C,	0xFFFFFF0D,	0xFFFFFF0E,	0xFFFFFF10,
	0xFFFFFF12,	0xFFFFFF13,	0xFFFFFF15,	0xFFFFFF17,	0xFFFFFF18,	0xFFFFFF1A,	0xFFFFFF1C,	0xFFFFFF1E,	0xFFFFFF21,	0xFFFFFF23,
	0xFFFFFF25,	0xFFFFFF27,	0xFFFFFF2A,	0xFFFFFF2C,	0xFFFFFF2F,	0xFFFFFF31,	0xFFFFFF34,	0xFFFFFF37,	0xFFFFFF3A,	0xFFFFFF3C,
	0xFFFFFF3F,	0xFFFFFF42,	0xFFFFFF45,	0xFFFFFF48,	0xFFFFFF4B,	0xFFFFFF4F,	0xFFFFFF52,	0xFFFFFF55,	0xFFFFFF59,	0xFFFFFF5C,
	0xFFFFFF5F,	0xFFFFFF63,	0xFFFFFF66,	0xFFFFFF6A,	0xFFFFFF6E,	0xFFFFFF71,	0xFFFFFF75,	0xFFFFFF79,	0xFFFFFF7D,	0xFFFFFF81,
	0xFFFFFF84,	0xFFFFFF88,	0xFFFFFF8C,	0xFFFFFF90,	0xFFFFFF94,	0xFFFFFF98,	0xFFFFFF9C,	0xFFFFFFA1,	0xFFFFFFA5,	0xFFFFFFA9,
	0xFFFFFFAD,	0xFFFFFFB1,	0xFFFFFFB6,	0xFFFFFFBA,	0xFFFFFFBE,	0xFFFFFFC3,	0xFFFFFFC7,	0xFFFFFFCB,	0xFFFFFFD0,	0xFFFFFFD4,
	0xFFFFFFD8,	0xFFFFFFDD,	0xFFFFFFE1,	0xFFFFFFE6,	0xFFFFFFEA,	0xFFFFFFEF,	0xFFFFFFF3,	0xFFFFFFF8,	0xFFFFFFFC,	0x00000000,
	0x00000004,	0x00000008,	0x0000000D,	0x00000011,	0x00000016,	0x0000001A,	0x0000001F,	0x00000023,	0x00000028,	0x0000002C,
	0x00000030,	0x00000035,	0x00000039,	0x0000003D,	0x00000042,	0x00000046,	0x0000004A,	0x0000004F,	0x00000053,	0x00000057,
	0x0000005B,	0x0000005F,	0x00000064,	0x00000068,	0x0000006C,	0x00000070,	0x00000074,	0x00000078,	0x0000007C,	0x00000080,
	0x00000083,	0x00000087,	0x0000008B,	0x0000008F,	0x00000092,	0x00000096,	0x0000009A,	0x0000009D,	0x000000A1,	0x000000A4,
	0x000000A7,	0x000000AB,	0x000000AE,	0x000000B1,	0x000000B5,	0x000000B8,	0x000000BB,	0x000000BE,	0x000000C1,	0x000000C4,
	0x000000C6,	0x000000C9,	0x000000CC,	0x000000CF,	0x000000D1,	0x000000D4,	0x000000D6,	0x000000D9,	0x000000DB,	0x000000DD,
	0x000000DF,	0x000000E2,	0x000000E4,	0x000000E6,	0x000000E8,	0x000000E9,	0x000000EB,	0x000000ED,	0x000000EE,	0x000000F0,
	0x000000F2,	0x000000F3,	0x000000F4,	0x000000F6,	0x000000F7,	0x000000F8,	0x000000F9,	0x000000FA,	0x000000FB,	0x000000FC,
	0x000000FC,	0x000000FD,	0x000000FE,	0x000000FE,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,	0x000000FF,
};


// global trig calculation vars
FIXED g_ffixXVel	= 0;
FIXED g_ffixYVel	= 0;
FIXED g_ffixCVel	= 0;
FIXED g_ffixTheta	= 0;

// calculate C velocity (hypotinuse) and Y velocity from X velocity and angle Theta
void CalcTrig_CY()
{
	s32 nTheta = FIXED_INT_PART(g_ffixTheta);

	// use trig identity C = X / Cos(Theta)
	g_ffixCVel = FIXED_DIVIDE(g_ffixXVel, COS[nTheta]);

	// use Y = X Sin(Theta) / Cos(Theta)
	g_ffixYVel = FIXED_MULTIPLY(g_ffixXVel, FIXED_DIVIDE( SIN[nTheta], COS[nTheta] ));
}

// calculate C velocity and X velocity from Y velocity and angle Theta
void CalcTrig_CX()
{
	s32 nTheta = FIXED_INT_PART(g_ffixTheta);

	// use trig identity C = Y / Sin(Theta)
	g_ffixCVel = FIXED_DIVIDE(g_ffixYVel, SIN[nTheta]);

	// use X = Y Cos(Theta) / Sin(Theta)
	g_ffixXVel = FIXED_MULTIPLY(g_ffixYVel, FIXED_DIVIDE( COS[nTheta], SIN[nTheta] ));
}

// calculate X velocity and Y velocity from the C velocity and angle Theta
void CalcTrig_XY()
{
	s32 nTheta = FIXED_INT_PART(g_ffixTheta);

	g_ffixXVel = FIXED_MULTIPLY(g_ffixCVel, COS[nTheta]);
	g_ffixYVel = FIXED_MULTIPLY(g_ffixCVel, SIN[nTheta]);
}
