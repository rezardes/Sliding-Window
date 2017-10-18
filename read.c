#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) 
{
	FILE *fin;
	char *buff, *str;
	int size;

	printf("Size buffer (byte) : ");
	scanf("%d", &size);
	size++;
	buff = malloc(size);

	fin = fopen("message.txt","r");
	if (fin == NULL) {
		printf("failed opening file\n");
	}
	else {
		// contoh penanganan buffer
		if ( fgets(buff, size, fin) ) {
			strcpy(str,buff);
		}
		while ( fgets(buff, size, fin) != NULL ) {
			strcat(str,buff);

		}
		printf("%s\n",str); //text tersimpan di string str
	}

	fclose(fin);

	return 0;
}