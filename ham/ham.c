/* ham - hamming error correction codes
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdbool.h>
#include <stdint.h>

#include "ham.h"

static inline bool pow2(int val);

/* returns true if a number is a power of two or zero */
static inline bool
pow2(int val)
{
	return (val & (val - 1)) == 0;
}

uint16_t
ham_enc(uint16_t val) /* val >> 11 == 0 */
{
	uint16_t code = 0;

	/* create spaces at 0, 1, 2, 4, 8th bits */
	for (int8_t i = 15, ind = 10; i > 0; --i) {
		if (!pow2(i)) {
			code |= ((val >> ind) & 1);
			--ind;
		}
		code <<= 1;
	}

	for (uint8_t i = 1; i <= 8; i *= 2) {
		uint8_t cnt = 0;
		for (uint8_t j = i + 1; j < 16; ++j)
			if (i & j)
				cnt += ((code >> j) & 1);
		code |= ((cnt & 1) << i);
	}

	uint8_t cnt = 0;
	for (uint8_t i = 0; i < 16; ++i)
		cnt += ((code >> i) & 1);
	code |= (cnt & 1);

	return code;
}

uint16_t
ham_dec(uint16_t code)
{
	uint8_t shift = 0;
	for (uint8_t i = 1; i <= 8; i *= 2) {
		uint8_t cnt = 0;
		for (uint8_t j = i + 1; j < 16; ++j)
			if (i & j)
				cnt += ((code >> j) & 1);

		shift |= ((cnt & 1) != ((code >> i) & 1)) << 4;
		shift >>= 1;
	}

	if (shift != 0) {
		uint8_t cnt = 0;
		for (uint8_t i = 1; i < 16; ++i)
			cnt += ((code >> i) & 1);
		if ((cnt & 1) == (code & 1))
			return (uint16_t)-1; /* two bit error */

		code ^= 1 << shift;
	}

	uint16_t val = 0;
	for (int8_t i = 15; i > 2; --i) {
		if (!pow2(i)) {
			val <<= 1;
			val |= (code >> i) & 1;
		}
	}

	return val;
}
