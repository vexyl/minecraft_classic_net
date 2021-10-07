#include "main.h"
#include "../stream_buffer.h"
#include "../tcp_socket.h"
#include "../net.h"
#include "../classic_protocol.h"
#include "../protocol.h" // for error codes

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

extern void (*classic_on_message)(struct message_packet packet);

void on_message(struct message_packet packet)
{
	char message[65];
	message[64] = '\0';
	strncpy(message, packet.message, 64);
	printf("[Message] %s\n", message);
}

int main(int argc, char* argv[])
{
	if (argc < 5) {
		fprintf(stderr, "not enough arguments\nusage: %s <ip> <port> <name> <key>\n", argv[0]);
		exit(0);
	}
	
	struct tcp_socket sock;
	struct net_protocol_handler proto_handler;

	char* ip = argv[1];
	short port = atoi(argv[2]);
	char name[64];
	char key[64];
	
	memset(name, 0x20, 64);
	memset(key, 0x20, 64);
	strncpy(name, argv[3], strlen(argv[3]));
	strncpy(key, argv[4], strlen(argv[4]));

	net_protocol_handler_initialize(&proto_handler);

	classic_on_message = &on_message;
	
	tcp_socket_initialize(&sock);
	tcp_socket_connect(&sock, ip, port);
	
	printf("connected\n");
	
	struct stream_buffer stream;
	stream_initialize(&stream, NULL, 0);
	stream_write_uint8(&stream, 0x00);
	stream_write_uint8(&stream, 0x07);
	stream_write_string64(&stream, name);
	stream_write_string64(&stream, key);
	stream_write_uint8(&stream, 0x00);

	tcp_socket_send_stream(&sock, &stream);
	stream_free(&stream);
	
	bool running = true;
	while(running) {
		tcp_socket_poll(&sock);
		
		int result = net_protocol_handler_handle_opcode(&proto_handler, &sock);
		if (result == PROTOCOL_INVALID_OPCODE) {
			uint8_t opcode = tcp_socket_peek_first_byte(&sock);
			fprintf(stderr, "unhandled opcode: 0x%.2x\n", opcode);
			exit(1);
		}
	}
	
	net_protocol_handler_free(&proto_handler);
	tcp_socket_close(&sock);
	return 0;
}
