#ifndef __KEYPAD__
#define __KEYPAD__

#define KEY_A			1
#define KEY_B			2
#define KEY_SELECT		4
#define KEY_START		8
#define KEY_RIGHT		16
#define KEY_LEFT		32
#define KEY_UP			64
#define KEY_DOWN		128
#define KEY_R			256
#define KEY_L			512

#define KEYS			(*(volatile u16*)0x4000130)

#define KEYDOWN(k)		( ! ( ( KEYS ) & k ) )

#endif