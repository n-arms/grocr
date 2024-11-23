/* ham - hamming error correction codes
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ham.h"

static void bin(uint16_t val);
static inline bool pow2(int val);

static void
bin(uint16_t val)
{
	for (int8_t i = 15; i >= 0; --i)
		printf("%c", (val >> i) & 1 ? '1' : '0');
	printf("\n");
}

static inline bool
pow2(int val)
{
	return (val & (val - 1)) == 0;
}

int
main(void)
{
	srand(time(NULL));
	uint16_t num = rand();
	num >>= 5; /* num is an eleven bit value */

	const uint16_t encoded = enc(num);
	for (uint16_t i = 1 << 15; i != 0; i >>= 1)
		if (dec(encoded ^ i) != num)
			bin(num), bin(encoded), bin(encoded ^ 1);

	for (uint16_t i = (1 << 15) + (1 << 11); !pow2(i); i >>= 1)
		if (dec(encoded ^ i) != 0)
			bin(num), bin(encoded), bin(encoded ^ 1);

	for (uint16_t i = (1 << 15) + (1 << 7); !pow2(i); i >>= 1)
		if (dec(encoded ^ i) != 0)
			bin(num), bin(encoded), bin(encoded ^ 1);

	printf("FEDCBA9876543210\n");
}
