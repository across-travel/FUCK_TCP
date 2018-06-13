#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

/*
Packet := header + body
header := length + '|';
length := [0-9]+
body   := text

Example:

01A
12Hello World!
*/

#define MAX_PACKET_SIZE 1024

char* encode_packet(const char *text);

int sock_server(const char *ip, int port);
int sock_accept(int serv_sock);
int sock_connect(const char *ip, int port);


char* encode_packet(const char *text){
	int body_len = strlen(text);
	if(body_len > MAX_PACKET_SIZE - 15){
		return NULL;
	}
	char header[20];
	int header_len = snprintf(header, sizeof(header), "%d|", body_len);
	int packet_len = header_len + body_len;

	char *buf = (char *)malloc(packet_len + 1);
	snprintf(buf, packet_len + 1, "%s%s", header, text);
	return buf;
}

int sock_server(const char *ip, int port){
	struct sockaddr_in addr;
	memset(&addr, sizeof(addr), 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);
	
	int serv_sock = 0;
	if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	int opt = 1;
	if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		goto sock_err;
	}
	if(bind(serv_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}
	if(listen(serv_sock, 1024) == -1){
		goto sock_err;
	}
	
	return serv_sock;
	
sock_err:
	if(serv_sock){
		close(serv_sock);
	}
	return -1;
}

int sock_accept(int serv_sock){
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int sock = accept(serv_sock, (struct sockaddr *)&addr, &addrlen);
	if(sock == -1){
		return -1;
	}
	
	char remote_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, remote_ip, sizeof(remote_ip));
	int remote_port = ntohs(addr.sin_port);
	
	printf("accept client from %s:%d\n", remote_ip, remote_port);
	
	int opt = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));

	return sock;
}

int sock_connect(const char *ip, int port){
	int sock;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}
	return sock;

sock_err:
	if(sock){
		close(sock);
	}
	return -1;
}
