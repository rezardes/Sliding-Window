#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) 
{
	FILE *fin;
	char *buff, *str;
	int size;

	FILE *fout;

	printf("Size buffer (byte) : ");
	scanf("%d", &size);
	size++;
	buff = malloc(size);

	fin = fopen("message.txt","r"); //buffer diambil dari file message.txt
	if (fin == NULL) {
		printf("failed opening file\n");
	}
	else {
		//fungsi write
		fout = fopen("msgreceived.txt","w");
		while ( fgets(buff, size, fin) != NULL) {
			fprintf(fout, "%s", buff);
		}
		fclose(fout);
	}

	fclose(fin);

	return 0;
}