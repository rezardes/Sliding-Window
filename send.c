#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_SEGMENT 200
#define BUFLEN 2048
#define window_size 4
#define SERVICE_PORT 21234
#define port_receiver 21000
#define port_sender 20000
#define BUFSIZE 2048

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

int main () {

	char soh;
	char stx;
	char etx;
	char seqnum;
	char data;
	char checksum;

	struct sockaddr_in myaddr, myaddr2, remaddr, remaddr2;
	socklen_t addrlen = sizeof(remaddr2);
	int fd, fd2, i, slen=sizeof(remaddr), recvlen;
	char *server = "127.0.0.1";	/* change this to use a different server */
	char buf[BUFLEN], ack[BUFLEN];
	char ltr[] = "My Apple is very good!";
	char segment[9];
	char temp[9];
	char daftar[MAX_SEGMENT][9];
	int sent[MAX_SEGMENT];

	int received_ACK = 0, num_segment, left, right;

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

	num_segment = strlen(ltr);

	for(int i=0;i<strlen(ltr);i++) {
		generateSegment(segment,i,ltr[i]);
		copySegment(daftar[i],segment);
	}

	/*for(int i=0;i<strlen(ltr);i++) {
		printf("%c\n",daftar[i][6]);
	}*/

	received_ACK = 0;
	left = 0; right = left+window_size-1;
	memset(sent, 0, sizeof sent);

	while ( received_ACK < num_segment ) {

		/*if (left <= sent <= right) {
			
			sending++;
			now++;
		}*/

		for(int i=left;i<=right;i++) {
			if (!sent[i]){
				if (sendto(fd, daftar[i], sizeof(daftar[i]), 0, (struct sockaddr *)&remaddr, slen)==-1)
					perror("sendto");
				printf("Paket %d terkirim\n",i);
				sent[i] = 1;
			}
		}

		// JIKA MENERIMA ACK:
		printf("waiting on port %d\n", port_sender);
		recvlen = recvfrom(fd2, ack, BUFSIZE, 0, (struct sockaddr *)&remaddr2, &addrlen);
		printf("ACK %d received!\n",valInt(ack));
		left = valInt(ack);
		received_ACK++;
		right = left+window_size-1;

		/*if (buf[0]) {
			printf("ACK %d received.\n");
		}
		if (buf[0] > 0) {
			//buf[recvlen] = 0;
			printf("received message: \"%s\"\n", buf);
		}*/

		/*if (recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen)) {
			sending--;
			left = next_seq;
			right = next_seq+window_size;
		}*/

		/*if ( waktu timeout ) {
			now -= window_size;
		}*/
	}

	close(fd);
	close(fd2);

	

	return 0;
}