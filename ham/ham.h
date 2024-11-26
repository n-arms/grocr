/* ham - hamming error correction codes
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef HAM_H
#define HAM_H

/* hamming encoding uses a series of parity bits at locations in the data to
 * check and fix single bit flips made during data transfer.  an additional
 * parity bit can be used to check for, but not fix, double bit flips.
 *
 * this specific implementation encodes eleven bits with four hamming parity
 * bits and one extra parity bit for an encoded size of sixteen bits.
 *
 * to encode, parity bits are placed at bit positions 0, 1, 2, 4, and 8.
 * the parity bits at positions 1, 2, 4, and 8 are defined as the parity of the
 * bits at positions which have the binary power of two of the above positions
 * in their binary representation.  bit position 0 has a parity bit of the
 * entire generated code.
 *
 * to decode, the parity bits at bit positions 1, 2, 4, and 8 are verified. if
 * the parity is incorrect, a one is placed at the bit position represented by
 * the position number (from highest to lowest), otherwise a zero.  the integer
 * constructed by these bits is the bit which was flipped.
 *
 * if the variable is non-zero and the parity bit at bit position zero is
 * accurate, a single bit flip is assumed.  otherwise, a double bit flip is
 * assumed and negative one is returned instead of a corrected value. */

/* hamming encodes an eleven bit value */
uint16_t ham_enc(uint16_t val);
/* hamming decodes an eleven bit value */
uint16_t ham_dec(uint16_t val);

#endif /* HAM_H */
