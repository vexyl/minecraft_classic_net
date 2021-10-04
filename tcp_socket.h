#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#define TCP_SOCKET_MAX_RECV_SIZE 4096

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct tcp_socket {
	int sockfd;
	unsigned char buffer[TCP_SOCKET_MAX_RECV_SIZE];
	size_t offset;
};

void tcp_socket_initialize(struct tcp_socket* sock);
void tcp_socket_close(struct tcp_socket* sock);

void tcp_socket_connect(struct tcp_socket* sock, char* ip, short port);

int tcp_socket_poll(struct tcp_socket* sock);
bool tcp_socket_consume(struct tcp_socket* sock, unsigned char* buffer, size_t size);

uint8_t tcp_socket_peek_first_byte(struct tcp_socket* sock);

int tcp_socket_send(struct tcp_socket* sock, unsigned char* buffer, size_t size);

#endif // TCP_SOCKET_H
