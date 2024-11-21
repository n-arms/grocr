#ifndef __DATA_PACKET_H
#define __DATA_PACKET_H

#include <stdbool.h>

typedef struct data_packet {
  bool *data;
  size_t length;
  size_t current_index;
} data_packet;

data_packet new_data_packet(bool *data, size_t length);
bool pop_data_packet(data_packet *packet);
bool data_packet_is_done(data_packet *packet);
void reset_data_packet(data_packet *packet, bool *new_data);
void push_data_packet(data_packet *packet, bool value);



#endif
