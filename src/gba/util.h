#ifndef __UTIL_H__
#define __UTIL_H__

#include "gba.h"

u8 rand8();

extern FIXED SIN[360];
extern FIXED COS[360];

// global trig calculation vars
extern FIXED g_ffixXVel;
extern FIXED g_ffixYVel;
extern FIXED g_ffixCVel;
extern FIXED g_ffixTheta;

// trig functions
void CalcTrig_CY();
void CalcTrig_CX();
void CalcTrig_XY();

#endif
