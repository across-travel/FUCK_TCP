/*****************************************************
 * The TCP socket client that receive simulate Partial Packet
 * and Merged Packets from TCP connection.
 * @author: ideawu
 * @link: http://www.ideawu.net/
 *****************************************************/
#include "inc.h"
#include "buffer.h"

char* parse(struct Buffer *buf);

int main(int argc, char **argv){
	if(argc <= 2){
		printf("Usage: %s ip port\n", argv[0]);
		exit(0);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	int sock = sock_connect(ip, port);
	if(sock == -1){
		printf("error: %s\n", strerror(errno));
		exit(0);
	}
	printf("connected to %s:%d\n", ip, port);
	
	struct Buffer buf;
	buffer_init(&buf);
	
	while(1){
		char tmp[128];
		int len = read(sock, tmp, sizeof(tmp));
		if(len <= 0){
			printf("receive %d, exit.\n", len);
			exit(0);
		}
		if(buffer_add(&buf, tmp, len) == -1){
			printf("error, too large packet!\n");
			exit(0);
		}
		
		int n = 0;
		while(1){
			char *msg = parse(&buf);
			if(!msg){
				break;
			}
			n ++;
			
			if(n == 1){
				printf("< %s\n", msg);
			}else{
				printf("< %s [Merged Packet]\n", msg);
			}
			free(msg);
		}
	}

	return 0;
}

char* parse(struct Buffer *buf){
	if(buf->size == 0){
		return NULL;
	}
	if(buf->size < PACKET_HEADER_SIZE){
		printf("[Partial Packet] header not ready, buffer %d\n", buf->size);
		return NULL;
	}
	
	char length[3];
	length[0] = buf->data[0];
	length[1] = buf->data[1];
	length[2] = '\0';
	int len = atoi(length);
	if(buf->size < PACKET_HEADER_SIZE + len){
		printf("[Partial Packet] body not ready, buffer %d\n", buf->size);
		return NULL;
	}
	
	char *body = malloc(len + 1);
	if(len > 0){
		memcpy(body, buf->data + PACKET_HEADER_SIZE, len);
	}
	body[len] = '\0';
	
	buffer_del(buf, PACKET_HEADER_SIZE + len);
	
	return body;
}

