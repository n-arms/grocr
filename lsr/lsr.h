/* lsr - least squares linear regression
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef LSR_H
#define LSR_H

/* least squares linear regression is a method to generate a line of best fit
 * for a series of data points. the algorithm used is as follows:
 *
 * m = (N*(s x*y) - (s x)*(s y))/(N*(s x^2) - (s x)^2)
 * b = ((s y) - m*(s x))/(N)
 *
 * where (s x) denotes a sum for all expressions involving points
 * where N denotes the number of points given
 *
 * the root of the linear regression is used to determine when the level should
 * be zero */

/* returns linear root of least squares linear regression of series of `len'
 * number of (`time', `level') ordered pairs */
float lsr_root(float *time, float *level, int len);

#endif /* LSR_H */
