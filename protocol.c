#include "protocol.h"
#include "stream_buffer.h"
#include "tcp_socket.h"

struct opcode_table_entry opcode_table_entry_new(void (*handler_func)(struct stream_buffer*), size_t packet_size)
{
	return (struct opcode_table_entry){ handler_func, packet_size };
}

void protocol_initialize(struct protocol* proto)
{
	for (int i = 0; i < MAX_OPCODES; ++i) {
		proto->opcode_table[i].handler_func = NULL;
		proto->opcode_table[i].packet_size = 0;
	}

}

int protocol_handle(struct protocol* proto, struct tcp_socket* sock, uint8_t opcode)
{
	if (proto->opcode_table[opcode].handler_func == NULL)
		return -1;
		
	int result = 0;
	size_t packet_size = proto->opcode_table[opcode].packet_size;
	struct stream_buffer stream;

	stream_initialize(&stream, NULL, packet_size);
	
	if (tcp_socket_consume(sock, stream.buffer, packet_size))
		proto->opcode_table[opcode].handler_func(&stream);
	else
		result = 1;
		
	stream_free(&stream);
	return result;
}
