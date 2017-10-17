#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// untuk mengubah char ke binary
static char *char_to_binary(unsigned char byte)
{
    char *result = malloc(9);
    if (result != 0)
    {
        char *digit = result;
        for (int i = 0; i < 8; i++)
            *digit++ = ((byte >> (7 - i)) & 0x01) + '0';
        *digit = '\0';
    }
    return result;
}

// untuk mengubah decimal ke binary
static char *decimal_to_binary(unsigned short byte) {
    char *result = malloc(9);
    if (result != 0)
    {
        char *digit = result;
        for (int i = 0; i < 8; i++)
            *digit++ = ((byte >> (7 - i)) & 0x01) + '0';
        *digit = '\0';
    }
    return result;
}

// untuk mengubah char ke binary
static char *int_to_binary(unsigned int byte) {
    char *result = malloc(33);
    if (result != 0)
    {
        char *digit = result;
        for (int i = 0; i < 32; i++)
            *digit++ = ((byte >> (31 - i)) & 0x01) + '0';
        *digit = '\0';
    }
    return result;
}

// membuat format segment
static char *createSegment(unsigned int seqnum, char d, char cs) 
{
    char *soh, *stx, *etx, *seqN, *data, *checkS;
    char *concatenated = malloc(73);

    soh = decimal_to_binary(1);
    stx = decimal_to_binary(2);
    etx = decimal_to_binary(3);

    seqN = int_to_binary(seqnum);
    data = char_to_binary(d);
    checkS = char_to_binary(cs);

    strcpy(&concatenated[0], soh);
    strcpy(&concatenated[8], seqN);
    strcpy(&concatenated[40], stx);
    strcpy(&concatenated[48], data);
    strcpy(&concatenated[56], etx);
    strcpy(&concatenated[64], checkS);

    return concatenated;
}

// membuat format ACK
static char *createACK(unsigned int nextSN, char advWS, char cs) 
{
    char *ack, *nextSeqNum, *windowSize, *checkS;
    char *concatenated = malloc(57);

    ack = decimal_to_binary(6);

    nextSeqNum = int_to_binary(nextSN);
    windowSize = char_to_binary(advWS);
    checkS = char_to_binary(cs);

    strcpy(&concatenated[0], ack);
    strcpy(&concatenated[8], nextSeqNum);
    strcpy(&concatenated[40], windowSize);
    strcpy(&concatenated[48], checkS);

    return concatenated;
}

// cek hasil createSegment dan createACK
/*
int main(void)
{

    printf("%s\n", createSegment(15, 'b', 'b'));
    printf("\n");
    printf("%s\n", createACK(15, 'b', 'b'));

    return(0);
}*/