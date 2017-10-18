#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "port.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFLEN 2048
#define port_sender 20000
#define port_receiver 21000
#define BUFSIZE 2048
#define MAX_SEGMENT 200
#define window_size 4

void generateACK(char ack[], int num, int adv_win) 
{
	/* Bagaimana untuk > 2^7 */
	/* Perbaiki checksum */
    ack[0] = 0x6;
	ack[1] = (num>>24) & 0xFF;
	ack[2] = (num>>16) & 0xFF;
	ack[3] = (num>>8) & 0xFF;
	ack[4] = num & 0xFF;
	ack[5] = adv_win;
	ack[6] = 0b00000000;
}

int valInt(char ltr[])
{
	int num;
	num = (int) ( (unsigned char) (ltr[1]) << 24 |
				(unsigned char) (ltr[2]) << 16 |
				(unsigned char) (ltr[3]) << 8 |
				(unsigned char) (ltr[4]));
	return num;
}

int main(void)
{
	struct sockaddr_in myaddr, remaddr, mysend;
	struct sockaddr_in addr;
	int fd_ack, fd_receive, i, slen=sizeof(remaddr), addrlen=sizeof(addr);
	int recvlen, capacity;
	char *server = "127.0.0.1";	/* UBAH AGAR KE SERVER BERBEDA */
	char buf[BUFLEN], ack[7];
	int occupancy[MAX_SEGMENT];
	srand(time(NULL));
	int left;
	int right;

	/* INISIALISASI */
	memset(occupancy,0,sizeof occupancy);
	left = 0; right = left+window_size-1;
	capacity = window_size;

	/* MEMBUAT SOCKET */
	if ((fd_ack=socket(AF_INET, SOCK_DGRAM, 0))<0)
		perror("socket can't be created!\n");

	if ((fd_receive=socket(AF_INET, SOCK_DGRAM, 0))<0)
		perror("socket can't be created\n");

	/* MEMBUAT PORT */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port_receiver);

	memset((char *)&mysend, 0, sizeof(mysend));
	mysend.sin_family = AF_INET;
	mysend.sin_addr.s_addr = htonl(INADDR_ANY);
	mysend.sin_port = htons(0);

	/* BINDING SOCKET */

	if (bind(fd_ack, (struct sockaddr *)&mysend, sizeof(mysend)) < 0) {
		perror("bind failed");
		return 0;
	} 

	if (bind(fd_receive, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	/* MENENTUKAN PORT TUJUAN */

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port_sender);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	/* ALGORITMA MENERIMA PAKET */

	for (;;) {
		printf("waiting on port %d\n", port_receiver);
		recvlen = recvfrom(fd_receive, buf, BUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
		printf("received %d bytes\n", recvlen);
		if (recvlen > 0) {
			printf("received message: \"%c\"\n", buf[6]);
		}
		occupancy[valInt(buf)] = 1;

		/* PENGIRIMAN ACK */
		printf("Sending ACK %d to port %d\n", valInt(buf)+1, port_sender);
		if (left == valInt(buf)) {
			generateACK(ack,left+1,window_size-1);
			left++;
			right = left + window_size - 1;
		} else if (valInt(buf)>left && valInt(buf)<=right) {
			generateACK(ack,left,window_size-1);
		}
		
		if (sendto(fd_ack, ack, sizeof(ack), 0, (struct sockaddr *)&remaddr, slen)==-1)
			perror("sendto");

		/* PENULISAN KE BUFFER */

		sleep(rand()%5);
	}

	close(fd_ack);
	close(fd_receive);

	return 0;
}


/*#include <stdio.h>

int main () {

	int RWS;

	RWS = window_size;
	LFR = -1;
	//seq_num_to_ACK =

	while ( true ) {
		 waktu tertentu tidak menerima data lagi dimatikan 
		if ( menerima paket ) {
			 sleep 
			if (LFR < seq_num <= LAF) {
				seq_num_to_ACK = LFR;
				makeACK(LFR);
				sendACK(LFR);
				LAF = RWS + LFR;
			}
			
		}
	} 
}*/