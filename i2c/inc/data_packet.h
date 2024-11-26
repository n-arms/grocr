#ifndef __DATA_PACKET_H
#define __DATA_PACKET_H

#include <stdbool.h>
#include <stdlib.h>

/*
This module implements the storage backing for a packet of data sent or received over I2C.

Example usage:
bool backing_array[10];
data_packet p = new_data_packet(backing_array, 10);
push_data_packet(&p);
bool bit = pop_data_packet(&p):
assert(bit == true);
assert(!data_packet_is_done(&p));
*/

typedef struct data_packet {
  bool *data;
  size_t length;
  size_t current_index;
} data_packet;

/* Create a new data packet. */
data_packet new_data_packet(bool *data, size_t length);

/* Remove a bit from the end of the data packet and return it. */
bool pop_data_packet(data_packet *packet);

/* Check if the data packet is full. */
bool data_packet_is_done(data_packet *packet);

/* Remove all elements from the data packet. */
void reset_data_packet(data_packet *packet, bool *new_data);

/* Push a bit to the data packet */
void push_data_packet(data_packet *packet, bool value);



#endif
