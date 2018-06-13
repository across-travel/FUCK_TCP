all:
	gcc -std=c99 -O2 -o server server.c
	gcc -std=c99 -O2 -o client client.c
