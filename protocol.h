#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define MAX_OPCODES 256

#include <stdlib.h>
#include <stdint.h>

struct stream_buffer;
struct tcp_socket;

enum PROTOCOL_ERRORS {
	PROTOCOL_INVALID_OPCODE = -1,
	PROTOCOL_PARTIAL_PACKET = 0,
	PROTOCOL_SUCCESS = 1
};

struct opcode_table_entry {
	void (*handler_func)(struct stream_buffer*);
	size_t packet_size;
};

struct opcode_table_entry opcode_table_entry_new(void (*handler_func)(struct stream_buffer*), size_t packet_size);

struct protocol {
	struct opcode_table_entry opcode_table[MAX_OPCODES];
};

void protocol_initialize(struct protocol* proto);

int protocol_handle(struct protocol* proto, struct tcp_socket* sock, uint8_t opcode);

#endif // PROTOCOL_H_
