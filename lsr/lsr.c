/* lsr - least squares linear regression
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include "lsr.h"

float
lsr_root(uint32_t *x, float *y, int len)
{
	float sx = 0, sx2 = 0, sy = 0, sxy = 0; /* sums */
	for (int i = 0; i < len; ++i) {
		sx += x[i];
		sx2 += x[i] * x[i];
		sy += y[i];
		sxy += x[i] * y[i];
	}

	const float m = (len * sxy - sx * sy) /
			(len * sx2 - sx * sx);
	const float b = (sy - m * sx) / len;
	return -b / m;
}
