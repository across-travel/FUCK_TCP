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
	int head_len;
	int body_len;

	{
		char *body = (char *)memchr(buf->data, '|', buf->size);
		if(body == NULL){
			printf("[Partial Packet] header not ready, buffer %d\n", buf->size);
			return NULL;
		}
		body ++;
		head_len = body - buf->data;
	}

	{
		char header[20];
		memcpy(header, buf->data, head_len - 1); // no '|'
		header[head_len - 1] = '\0';
		body_len = atoi(header);
	}

	if(buf->size < head_len + body_len){
		printf("[Partial Packet] body not ready, buffer %d\n", buf->size);
		return NULL;
	}
	
	char *body = malloc(body_len + 1);
	if(body_len > 0){
		memcpy(body, buf->data + head_len, body_len);
	}
	body[body_len] = '\0';
	
	buffer_del(buf, head_len + body_len);
	
	return body;
}

