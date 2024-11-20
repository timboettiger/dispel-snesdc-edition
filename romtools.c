#include "dispel.h"

unsigned long ConvertToFastROM(unsigned long address) {
    return address | 0x800000; // Setzt das FastROM-Bit
}

unsigned long ConvertToSlowROM(unsigned long address) {
    return address & ~0x800000; // Entfernt das FastROM-Bit
}

int ScoreHiROM(unsigned char **data)
{
	int score = 0;

	if (((*data)[0xFFDC] + (*data)[0xFFDD]*256 + (*data)[0xFFDE] + (*data)[0xFFDF]*256) == 0xFFFF)
	{
		score += 2;
	}

	if ((*data)[0xFFDA] == 0x33)
	{
		score += 2;
	}
	if (((*data)[0xFFD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!((*data)[0xFFFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << ((*data)[0xFFD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&(*data)[0xFFB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&(*data)[0xFFC0], 20))
	{
		score -= 1;
	}

	return (score);
}

int ScoreLoROM(unsigned char **data)
{
	int score = 0;

	if (((*data)[0x7FDC] + (*data)[0x7FDD]*256 + (*data)[0x7FDE] + (*data)[0x7FDF]*256) == 0xFFFF)
	{
		score += 2;
	}
	if ((*data)[0x7FDA] == 0x33)
	{
		score += 2;
	}
	if (((*data)[0x7FD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!((*data)[0x7FFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << ((*data)[0x7FD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&(*data)[0x7FB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&(*data)[0x7FC0], 20))
	{
		score -= 1;
	}

	return (score);
}

void autoDetectHiLoROM(unsigned char **data, unsigned char *hirom) {
    int hiscore = ScoreHiROM(data);
    int loscore = ScoreLoROM(data);
    *hirom = (hiscore > loscore) ? 1 : 0;
}

void calculateAddressOptions(unsigned char **data, unsigned long *start, unsigned long *end, unsigned long *pos, unsigned long origin, unsigned char *hirom, unsigned char *shadow, unsigned int *bank, unsigned long len) {
    // Set initial position to the start address
    *pos = *start;

    // If shadow addresses are provided, adjust them
    if ((*bank == 0x100 && (*start & 0x800000)) || (*bank & 0x80)) {
        *shadow = 1;
    }

    // If HiROM addresses are provided, adjust them
    if ((*bank == 0x100 && (*start & 0x400000)) || (*bank & 0x40)) {
        *hirom = 1;
    }
    // Mask the bank and adjust the start/end addresses to 24-bit unshadowed
    *bank &= 0x13F;
    *start &= 0x3FFFFF;
    *end &= 0x3FFFFF;

    // Detect shadow status if not explicitly set
    if (*shadow == 2) {
        if (*hirom) {
            *shadow = ((*data)[0xFFD5] & 0x30) ? 1 : 0;
        } else {
            *shadow = ((*data)[0x7FD5] & 0x30) ? 1 : 0;
        }
    }

    // If the bank is explicitly set, calculate start/end addresses accordingly
    if (*bank < 0x100) {
        if (*hirom) {
            *pos = *bank << 16;
            *start = *pos;
            *end = *start | 0xFFFF;
        } else {
            *pos = (*bank << 16) + 0x8000;
            *start = *bank * 0x8000;
            *end = *start + 0x7FFF;
        }
    } else {
        // Convert LoROM addresses to file offsets
        if (!hirom) {
            if ((*start & 0xFFFF) < 0x8000) {
                *start += 0x8000;
            }
            *pos = *start;
            *start = ((*start >> 16) & 0xFF) * 0x8000 + (*start & 0x7FFF);
            *end = ((*end >> 16) & 0xFF) * 0x8000 + (*end & 0x7FFF);
        }
    }

    // If the end address is invalid or before the start, set it to the end of the file
    if (*end <= *start) {
        *end = len - 1;
    }

    // Apply the origin address if provided
    if (origin < 0x1000000) {
        *pos = origin;
    }

    // Apply shadow adjustments
    if (*shadow) {
        *pos |= 0x800000;
    }

    // Apply HiROM mapping if enabled
    if (*hirom) {
        *pos |= 0x400000;
    }
}
