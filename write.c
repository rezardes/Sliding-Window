#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) 
{
	char *msg = "halo halo bandung ibu kota periangan.";
	char *buff;
	int size, i;
	FILE *fout;

	printf("Size buffer (byte) : ");
	scanf("%d", &size);
	buff = malloc(size+1);
	
	//fungsi write
	fout = fopen("msgreceived.txt","w");
	i = 0;
	while ( i < strlen(msg)) {
		strncpy(buff, msg + i, size);
		printf("%s\n", buff);
		fprintf(fout, "%s", buff);
		i = i + size;
	}
	fclose(fout);

	return 0;
}