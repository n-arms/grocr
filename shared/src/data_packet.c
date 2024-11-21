#include "data_packet.h"

#include <string.h>

data_packet new_data_packet(bool *data, size_t length) {
  data_packet result;
  result.data = data;
  result.length = length;
  result.current_index = current_index;
}

bool pop_data_packet(data_packet *packet) {
	if (packet -> length == packet -> current_index) {
		return false;
	}
	return (packet -> data)[packet -> current_index++];
}

bool data_packet_is_done(data_packet *packet) {
	return packet -> length <= packet -> current_index;
}

void reset_data_packet(data_packet *packet, bool *new_data) {
	memcpy(packet -> data, new_data, packet -> length);
	packet -> current_index = 0;
}

void push_data_packet(data_packet *packet, bool value) {
	if (packet -> length != packet -> current_index) {
		(packet -> data)[packet -> current_index++] = value;
	}
}

