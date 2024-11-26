/* ham - hamming error correction codes
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef HAM_H
#define HAM_H

/* hamming encoding uses a series of parity bits at specific positions in the
 * data to check for single bit flips. an additional parity bit can be used to
 * check for double bit flips.
 *
 * this specific implementation encodes eleven bits with four hamming parity
 * bits and one extra parity bit to have an encoded size of sixteen bits.
 *
 * to encode, creates spaces in the value at bit positions 0, 1, 2, 4, and 8.
 * for bit positions 1, 2, 4, and 8, they are defined as the parity of the
 * specific bits of the bits in the spaced value which have the specific power
 * of two in their bit pattern. for bit position 0, that is the parity of the
 * entire code after the other parity bits have been populated.
 *
 * to decode, the parity bits at the bit positions 1, 2, 4, and 8 are verified.
 * a zero is added to the specific power of two bit position in a new varible
 * if the parity checks match, and a one is otherwise. this variable is the
 * position of the bit which was flipped if it is non-zero.
 *
 * if the variable is non-zero and the parity bit at bit position zero is
 * accurate, a single bit flip is assumed, otherwise a double bit flip is
 * assumed and negative one is returned instead of a corrected value. */

/* hamming encodes an eleven bit value */
uint16_t enc(uint16_t val);

/* hamming decodes an eleven bit value */
uint16_t dec(uint16_t val);

typedef struct ham {
	uint16_t msg1, msg2, msg3;
} ham_t;

/* hamming encodes a thirty-two bit value */
ham_t enc32(uint32_t val);

/* hamming decodes a thirty-two bit value */
uint32_t dec32(ham_t ham);


#endif /* HAM_H */
