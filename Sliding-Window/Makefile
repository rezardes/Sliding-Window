all: sendfile recvfile

sendfile: src/send.c
	gcc -o sendfile src/send.c -lpthread
recvfile: src/receive.c
	gcc -o recvfile src/receive.c -lpthread