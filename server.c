/*****************************************************
 * The TCP socket server to simulate Partial Packet
 * and Merged Packets transfer over TCP connection.
 * @author: ideawu
 * @link: http://www.ideawu.net/
 *****************************************************/
#include "inc.h"

void send_partial_packet(int sock);
void send_merged_packet(int sock);
void send_packets(int sock);

int main(int argc, char **argv){
	if(argc <  2){
		printf("Usage: %s port\n", argv[0]);
		exit(0);
	}
	int test = 0;
	if(argc > 2){
		test = 1;
		printf("enable test mode\n");
	}

	const char *ip = "0.0.0.0";
	int port = atoi(argv[1]);
	
	int serv = sock_server(ip, port);
	if(serv == -1){
		printf("error: %s\n", strerror(errno));
		exit(0);
	}
	printf("server listen on %s:%d\n", ip, port);
	
	while(1){
		int sock = sock_accept(serv);
		if(sock == -1){
			printf("error: %s\n", strerror(errno));
			exit(0);
		}
	
		if(test){
			send_partial_packet(sock);
			send_merged_packet(sock);
		}else{
			send_packets(sock);
		}
		close(sock);
		printf("data sent\n");
	}
}

void send_packets(int sock){
	char *p;
	int n;

	p = encode_packet("test");
	n = strlen(p);
	write(sock, p, n);
	free(p);
	usleep(10 * 1000);

	p = encode_packet("Hello");
	n = strlen(p);
	write(sock, p, n);
	free(p);
	usleep(10 * 1000);

	p = encode_packet("World!");
	n = strlen(p);
	write(sock, p, n);
	free(p);
	usleep(10 * 1000);
}

void send_partial_packet(int sock){
	char *p = encode_packet("test");
	int n = strlen(p);
	for(int i=0; i<n; i++){
		write(sock, p+i, 1);
		usleep(20 * 1000);
	}
	free(p);
}

void send_merged_packet(int sock){
	char buf[1024];
	char *p;
	int n;
	
	buf[0] = '\0';
	char *ptr = buf;
	
	p = encode_packet("Hello");
	n = strlen(p);
	strcpy(ptr, p);
	free(p);
	ptr += n;
	
	p = encode_packet("World!");
	n = strlen(p);
	strcpy(ptr, p);
	free(p);
	ptr += n;
	
	write(sock, buf, strlen(buf));
}

