#ifndef INTERUPT_H
#define INTERUPT_H

#include "gba.h"

#ifndef INTERUPT_C
void EnableInterupts(u16 interupts);
void DisableInterupts(u16 interupts);
#endif  // INTERUPT_C

#define INT_VBLANK          0x0001
#define INT_HBLANK          0x0002  
#define INT_VCOUNT          0x0004  
#define INT_TIMMER0         0x0008
#define INT_TIMMER1         0x0010
#define INT_TIMMER2         0x0020  
#define INT_TIMMER3         0x0040
#define INT_COMUNICATION    0x0080
#define INT_DMA0            0x0100
#define INT_DMA1            0x0200
#define INT_DMA2            0x0400
#define INT_DMA3            0x0800
#define INT_KEYBOARD        0x1000
#define INT_CART            0x2000
#define INT_ALL             0x4000

#endif  // INTERUPT_H
