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
	if (handler->protocol_list.head == NULL)
		return -1;
		
	int result = 0;
	uint8_t opcode = tcp_socket_peek_first_byte(sock);
	struct protocol* proto = (struct protocol*)(linked_list_next(&handler->protocol_list)->data);
	while (proto != NULL) {
		result = protocol_handle(proto, sock, opcode);
		if (result != -1)
			break;
			
		struct linked_list_node* node = linked_list_next(&handler->protocol_list);
		if (node == NULL)
			break;
			
		proto = (struct protocol*)node->data;
	}
	linked_list_reset(&handler->protocol_list);

	return result;
}
