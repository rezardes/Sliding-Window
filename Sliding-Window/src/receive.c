#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include "port.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFLEN 2048
#define port_sender 20000
//#define port_receiver 21000
#define BUFSIZE 2048
#define MAX_SEGMENT 200
//#define window_size 4
//#define buffer_size 3

/* SETIAP MENERIMA PAKET, GESER KE LOWEST UNOCCUPANT
*/

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

int main(int argc, char* argv[])
{
	struct sockaddr_in myaddr, remaddr, mysend;
	struct sockaddr_in addr;
	int fd_ack, fd_receive, i, slen=sizeof(remaddr), addrlen=sizeof(addr);
	int recvlen, capacity;
	char *server = "127.0.0.1";	/* UBAH AGAR KE SERVER BERBEDA */
	char buf[BUFLEN], ack[7];
	int occupancy[MAX_SEGMENT];
	int window_size, buffer_size;
	char receive_buffer[BUFLEN];
	char out;
	srand(time(NULL));
	int left, idx=0;
	int right, timing=0;
	int count=0, port_receiver;
	FILE* fout;
	char file_name[32];

	/* INISIALISASI */
	memset(occupancy,0,sizeof occupancy);
	left = 0; right = left+window_size-1;
	capacity = window_size;
	strcpy(file_name,argv[1]);
	window_size = atoi(argv[2]);
	buffer_size = atoi(argv[3]);
	port_receiver = atoi(argv[4]);

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

	/* BUKA FILE EKSTERNAL UNTUK WRITE */
	fout = fopen(file_name,"w");

	char kalimat[4];
	char mid, last;

	kalimat[0] = 'a';
	kalimat[2] = 'c';
	mid = kalimat[1];
	last = kalimat[3];

	/* ALGORITMA MENERIMA PAKET */

	for (;;) {
		printf("waiting on port %d\n", port_receiver);
		recvlen = recvfrom(fd_receive, buf, BUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
		printf("received %d packet %d bytes\n", valInt(buf), recvlen);
		if (recvlen > 0) {
			printf("received message: \"%c\"\n", buf[6]);
			if (buf[0]=='F') {

				for(int i=idx;count>0;i++) {
					//printf("Hasil: %c\n",receive_buffer[i]);
					if (receive_buffer[i]!=mid && receive_buffer[i]!=0 && receive_buffer[i]!=last) {
						fprintf(fout,"%c",receive_buffer[i]);
					}
					count--;
				}
				break;
			} else {
			    /*itoa(buf[6],t,2);
			    if (strlen(t)<8){
			      char fullZero[]="00000000";
			      char subZero[8];
			      memcpy(subZero,fullZero,8-strlen(t));
			      strcat(subZero,t);
			      strcpy(t,subZero);
			    }

			    a=strlen(t);
			    for(e=a;e<a+N-1;e++)
			        t[e]='0';

			    crc();

			    for(e=a;e<a+N-1;e++)
        		t[e]=cs[e-a];*/
			}
			//fprintf(stderr, "%s\n", );
			if (!occupancy[valInt(buf)]) {
				count = count + 1;
				occupancy[valInt(buf)] = 1;
				receive_buffer[valInt(buf)] = buf[6];
				if (count == buffer_size) {
					for(int i=idx;count>0;i++) {
						//printf("Hasil: %c\n",receive_buffer[i]);
						if (receive_buffer[i]!=mid && receive_buffer[i]!=0 && receive_buffer[i]!=last) {
							fprintf(fout,"%c",receive_buffer[i]);
							count--;
							idx++;
						} else {
							idx = i;
						}
						
					}
					//idx++;
					//fclose(fout);
				}
			} 
			//receive_buffer[valInt(buf)] = buf[6];
			for(int i=left;i<=right;i++) {
				if (!occupancy[i]){
					left = i;
					right = left + window_size - 1;
					break;
				}
			}

			generateACK(ack,left,window_size-1);
			sleep(rand()%10);
			printf("Sending ACK %d to port %d\n", left, port_sender);
			if (sendto(fd_ack, ack, sizeof(ack), 0, (struct sockaddr *)&remaddr, slen)==-1)
				perror("sendto");

			//if (left == right && occupancy[right])
		}

		
		//

		/* PENGIRIMAN ACK */
		
		/*if (valInt(buf)>= left && valInt(buf) <= right) {
			for(int i=left;i<=right;i++) {
				if (!occupancy[i]){
					left = i;
					right = left + window_size - 1;
					break;
				}
			}
			generateACK(ack,left,window_size-1);
			printf("Sending ACK %d to port %d\n", left, port_sender);
			if (sendto(fd_ack, ack, sizeof(ack), 0, (struct sockaddr *)&remaddr, slen)==-1)
				perror("sendto");
		}*/
		
		
		
		/*if (left == valInt(buf)) {
			generateACK(ack,left+1,window_size-1);
			left++;
			right = left + window_size - 1;
		} else if (valInt(buf)>left && valInt(buf)<=right) {
			generateACK(ack,left,window_size-1);
		}*/

		/* PENULISAN KE BUFFER */
	}

	fclose(fout);

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