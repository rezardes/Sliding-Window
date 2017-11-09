#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_SEGMENT 200
#define BUFLEN 2048
//#define window_size 4
#define SERVICE_PORT 21234
//#define port_receiver 21000
#define port_sender 20000
#define BUFSIZE 2048
#define TIMEOUT 100

/* PEMERIKSAAN: (WHITE TESTING)
1) SETIAP MENERIMA ACK, GESER SLIDING WINDOW */

void generateSegment(char segment[], int num,char data) 
{
	/* Bagaimana untuk > 2^7 */
    segment[0] = 0x1;
	segment[1] = (num>>24) & 0xFF;
	segment[2] = (num>>16) & 0xFF;
	segment[3] = (num>>8) & 0xFF;
	segment[4] = num & 0xFF;
	segment[5] = 0x2;
	segment[6] = data;
	segment[7] = 0x3;
	segment[8] = 0b00000000;
}

void copySegment(char segment1[], char segment2[]) 
{
    segment1[0] = segment2[0];
	segment1[1] = segment2[1];
	segment1[2] = segment2[2];
	segment1[3] = segment2[3];
	segment1[4] = segment2[4];
	segment1[5] = segment2[5];
	segment1[6] = segment2[6];
	segment1[7] = segment2[7];
	segment1[8] = segment2[8];
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

void partition(char ltr[], char* daftar[]) {
	
}

struct sockaddr_in remaddr2, remaddr;
char daftar[MAX_SEGMENT][9];
int sent[MAX_SEGMENT], num_segment;
int fd, fd2, recvlen, left, right, received_ACK, slen=sizeof(remaddr);
int window_size, port_receiver, buffer_size;
char ack[BUFLEN], destination_ip[BUFLEN];
socklen_t addrlen = sizeof(remaddr2);
time_t process_time[MAX_SEGMENT], starting;
int length, count;
char finishing[2];

void *receiveACK(void *vargp) {
	while ( 1 ) {
		recvlen = recvfrom(fd2, ack, BUFSIZE, 0, (struct sockaddr *)&remaddr2, &addrlen);
		printf("ACK %d received!\n",valInt(ack));
		
		left = valInt(ack);
		if ((left+window_size-1)<=num_segment) {
			right = (left+window_size-1);
		}
		
		printf("SWS: %d %d\n",left,right);
	}
}

void *sendPacket(void *vargp) {

	time_t start;
	while ( 1 ) {
		for(int i=left;i<=right;i++) {
			if (!sent[i]){
				if (sendto(fd, daftar[i], sizeof(daftar[i]), 0, (struct sockaddr *)&remaddr, slen)==-1)
					perror("sendto");
				printf("Paket %d terkirim\n",i);
				time(&start);
				process_time[i] = start;
				sent[i] = 1;
				count = 0;
			}
		}
		count++;
		if (left==right && count==5) {
			finishing[0] = 'F';
			if (sendto(fd, finishing, sizeof(finishing), 0, (struct sockaddr *)&remaddr, slen)==-1)
				perror("sendto");
		}
	}
}

void *processTime(void *vargp) {
	time_t timer;
	while ( 1 ) {
		for(int i=left;i<=right;i++) {
			time(&timer);
			if (process_time[i]!=starting && difftime(timer,process_time[i])>=TIMEOUT) {
				sent[i] = 0;
				printf("timeout paket %d!\n",i);
				process_time[i] = starting;
			}
		}
	}
	
}

/*void *stopwatch(void *vargp) {
	time_t timer, now;
	time(&now);
	while ( left <= right ) {
		time(&timer);
		if (difftime(now,timer)>=TIMEOUT) {
			sent[i] = 0;
			time(&now);
			printf("")
		}
	}
	
}*/

int main (int argc, char* argv[]) {

	char soh;
	char stx;
	char etx;
	char seqnum;
	char data;
	char checksum;
	char str[BUFLEN], temps[BUFLEN];
	int size = 1000;

	FILE *fin; 

	struct sockaddr_in myaddr, myaddr2;
	int i;
	char *server = "127.0.0.1";	/* change this to use a different server */
	char buf[BUFLEN];
	char ltr[] = "My Apple is very good!";
	length = strlen(ltr);
	//printf("%d\n",length);
	char segment[9];
	char temp[9];

	pthread_t tid[3];

	time(&starting);

	char file_name[32];

	strcpy(file_name,argv[1]);

	/* BUKA FILE EKSTERNAL UNTUK READ */
	fin = fopen(file_name,"r");
	window_size = atoi(argv[2]);
	buffer_size = atoi(argv[3]);
	printf("WS:%d",window_size);
	printf("BS:%d",buffer_size);
	strcpy(destination_ip,argv[4]);
	port_receiver = atoi(argv[5]);

	if (fin == NULL) {
		printf("failed opening file\n");
	}
	else {
		// contoh penanganan buffer
		if ( fgets(temps, size, fin) ) {
			strcpy(str,temps);
		}
		while ( fgets(temps, size, fin) != NULL ) {
			strcat(str,temps);

		}
		printf("%s\n",str); //text tersimpan di string str
	}

	fclose(fin);

	// /printf("%c\n",(char)56);
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	if ((fd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		//return 0;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	myaddr2.sin_family = AF_INET;
	myaddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr2.sin_port = htons(port_sender);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	if (bind(fd2, (struct sockaddr *)&myaddr2, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}  

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port_receiver);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}    

	num_segment = strlen(str);

	for(int i=0;i<strlen(str);i++) {
		generateSegment(segment,i,str[i]);
		copySegment(daftar[i],segment);
		process_time[i] = starting;
	}

	/*for(int i=0;i<strlen(ltr);i++) {
		printf("%c\n",daftar[i][6]);
	}*/

	received_ACK = 0;
	left = 0; right = left+window_size-1;
	memset(sent, 0, sizeof sent);

	pthread_create(&tid[2], NULL, processTime, NULL);
	pthread_create(&tid[0], NULL, sendPacket, NULL);
	pthread_create(&tid[1], NULL, receiveACK, NULL);
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	/*while ( received_ACK < num_segment ) {

		if (left <= sent <= right) {
			
			sending++;
			now++;
		}
		//sendPacket();
		

		// JIKA MENERIMA ACK:
		//printf("waiting on port %d\n", port_sender);
		//receiveACK();
	}*/

	close(fd);
	close(fd2);

	

	return 0;
}