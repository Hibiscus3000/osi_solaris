
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	unsigned char* buf = (unsigned char*)malloc(BUFSIZ * sizeof(unsigned char));
	int numberOfCharsRead;
	if (1 > (numberOfCharsRead = read(0,buf,BUFSIZ - 1)))
	{
		perror("Wasn't able to read line");
		free(buf);
		return 1;
	}
	buf[numberOfCharsRead] = 0;
	int i;
	if (1 > write(1,buf,numberOfCharsRead))
	{
		perror("Wasn't able to write line");
		free(buf);
		return 1;
	}
	free(buf);
	return 0;
}
