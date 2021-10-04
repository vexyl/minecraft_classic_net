#include "tcp_socket.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void tcp_socket_initialize(struct tcp_socket* sock)
{
#ifdef _WIN32
	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "Failed to initialize TCPSocket: WSAStartup failed\n");
		exit(1);
	}
#endif

	sock->offset = 0;
}

void tcp_socket_close(struct tcp_socket* sock)
{
#ifdef _WIN32
	WSACleanup();
#endif

	close(sock->sockfd);
}

void tcp_socket_connect(struct tcp_socket* sock, char* ip, short port)
{
	struct sockaddr_in addr;
	
	sock->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->sockfd == -1) {
		fprintf(stderr, "failed to create TCP socket\n");
		exit(0);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	
	if (connect(sock->sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
		fprintf(stderr, "failed to connect to %s:%d\n", ip, port);
		exit(0);
	}
	
#ifdef _WIN32
	// Async (non-blocking) sockets
	u_long mode = 1;
	ioctlsocket(sock->sockfd, FIONBIO, &mode);
#else
	fcntl(sock->sockfd, F_SETFL, fcntl(sock->sockfd, F_GETFL, 0) | O_NONBLOCK);
#endif

	// Disable Nagle's algorithm
	int flag = 1;
	setsockopt(sock->sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
}

int tcp_socket_poll(struct tcp_socket* sock)
{
	int max_recv_size = sizeof(sock->buffer) - sock->offset;
	int recv_size = -1;
	if (max_recv_size > 0) {
		recv_size = recv(sock->sockfd, &sock->buffer[sock->offset], max_recv_size, 0);
		if (recv_size > 0)
			sock->offset += recv_size;
	}
	
	return recv_size;
}

bool tcp_socket_consume(struct tcp_socket* sock, unsigned char* buffer, size_t size)
{
	if (sock->offset < size)
		return false;

	if (buffer != NULL)
		memcpy(buffer, sock->buffer, size);
		
	memmove(&sock->buffer[0], &sock->buffer[size], sock->offset - size);
	
	sock->offset -= size;
	return true;
}

uint8_t tcp_socket_peek_first_byte(struct tcp_socket* sock)
{
	return sock->buffer[0];
}

int tcp_socket_send(struct tcp_socket* sock, unsigned char* buffer, size_t size)
{
	int result = send(sock->sockfd, buffer, size, 0);
	assert(result == size);
	return result;
}
