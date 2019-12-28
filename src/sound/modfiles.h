#ifndef __MODFILES_H__
#define __MODFILES_H__

#define NUM_MOD_FILES	2

u8 currentModFile = 0;

extern const unsigned char _binary_bladeraver_bin_start[];
extern const unsigned char _binary_teknopekno_bin_start[];

enum
{
	MOD_BLADERAVER = 0,
	MOD_TEKNOPEKNO,
};

const u8* modfiles[NUM_MOD_FILES] = {
	_binary_bladeraver_bin_start,
	_binary_teknopekno_bin_start,
};

// pass a MOD_ enum to this function
const u8* GetModFile(u8 mod)
{
	return modfiles[mod];
}

#endif
