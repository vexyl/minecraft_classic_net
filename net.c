#include "net.h"
#include "tcp_socket.h"
#include "protocol.h"
#include "classic_protocol.h"

#include <stdlib.h>

void net_protocol_handler_initialize(struct net_protocol_handler* handler)
{
	struct protocol classic;

	linked_list_initialize(&handler->protocol_list, sizeof(struct protocol));

	classic_protocol_initialize(&classic);
	linked_list_add(&handler->protocol_list, &classic);
}

void net_protocol_handler_free(struct net_protocol_handler* handler)
{
	linked_list_free(&handler->protocol_list);
}

int net_protocol_handler_handle_opcode(struct net_protocol_handler* handler, struct tcp_socket* sock)
{
	int result = 0;
	uint8_t opcode = tcp_socket_peek_first_byte(sock);
	struct protocol* proto = (struct protocol*)linked_list_next(&handler->protocol_list);
	assert(proto != NULL);
	while (proto != NULL) {
		result = protocol_handle(proto, sock, opcode);
		if (result != -1)
			break;
		proto = (struct protocol*)(linked_list_next(&handler->protocol_list)->data);
	}
	linked_list_reset(&handler->protocol_list);

	return result;
}
