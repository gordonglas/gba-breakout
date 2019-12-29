#ifndef __MODFILES_H__
#define __MODFILES_H__

#define NUM_MOD_FILES	6

u8 currentModFile = 0;

// these names are based off the base file name (see make.bat)
// only 4 channel mods are supported
extern const unsigned char _binary_bladeraver_bin_start[];
extern const unsigned char _binary_teknopekno_bin_start[];
extern const unsigned char _binary_defloration_bin_start[];
extern const unsigned char _binary_mywonderland_bin_start[];
extern const unsigned char _binary_tint_bin_start[];
extern const unsigned char _binary_adrenochrome_bin_start[];

enum
{
	MOD_BLADERAVER = 0,
	MOD_TEKNOPEKNO,
	MOD_DEFLORATION,
	MOD_MYWONDERLAND,
	MOD_TINT,
	MOD_ADRENOCHROME,
};

const u8* modfiles[NUM_MOD_FILES] = {
	_binary_bladeraver_bin_start,
	_binary_teknopekno_bin_start,
	_binary_defloration_bin_start,
	_binary_mywonderland_bin_start,
	_binary_tint_bin_start,
	_binary_adrenochrome_bin_start,
};

// pass a MOD_ enum to this function
const u8* GetModFile(u8 mod)
{
	return modfiles[mod];
}

#endif
