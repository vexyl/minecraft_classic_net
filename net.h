#ifndef NET_H_
#define NET_H_

#include "linked_list.h"

struct tcp_socket;

enum NET_ERRORS {
	NET_INVALID_OPCODE = -1,
	NET_PARTIAL_PACKET = 0,
	NET_SUCCESS = 1
};

struct net_protocol_handler {
	struct linked_list protocol_list;
};

void net_protocol_handler_initialize(struct net_protocol_handler* handler);
void net_protocol_handler_free(struct net_protocol_handler* handler);
int net_protocol_handler_handle_opcode(struct net_protocol_handler* handler, struct tcp_socket* sock);

#endif // NET_H_
